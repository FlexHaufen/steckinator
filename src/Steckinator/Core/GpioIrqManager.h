/**
 * @file GpioIrqManager.h
 * @brief Global IRQ manager 
 * @version 0.1
 * @date 2026-05-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

// *** INCLUDES ***
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/sync.h>

#include <functional>
#include <unordered_map>
#include <array>

// *** NAMESPACE ***
namespace Steckinator {


    class GpioIrqManager {
    public:
        using Callback = std::function<void(uint gpio, uint32_t events)>;

        // Deleted copy/move — singleton
        GpioIrqManager(const GpioIrqManager&) = delete;
        GpioIrqManager& operator=(const GpioIrqManager&) = delete;

        static GpioIrqManager& instance() {
            static GpioIrqManager inst;
            return inst;
        }

        /**
         * Register (or replace) a callback for a specific GPIO pin.
         * @param gpio   Pin number
         * @param events GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL | …
         * @param cb     Handler — called from IRQ context, keep it short!
         */
        void registerCallback(uint gpio, uint32_t events, Callback cb) {
            uint32_t saved = spin_lock_blocking(lock_);
            callbacks_[gpio] = std::move(cb);
            spin_unlock(lock_, saved);

            gpio_set_irq_enabled(gpio, events, true);

            // Ensure the single global dispatcher is installed exactly once
            if (!installed_) {
                gpio_set_irq_enabled_with_callback(gpio, events, true, &GpioIrqManager::dispatch);
                installed_ = true;
            }
        }

        /**
         * Remove the callback for a GPIO and disable its IRQ.
         */
        void unregisterCallback(uint gpio) {
            gpio_set_irq_enabled(gpio, 0xFF, false);  // disable all events on pin

            uint32_t saved = spin_lock_blocking(lock_);
            callbacks_.erase(gpio);
            spin_unlock(lock_, saved);
        }

    private:
        GpioIrqManager() {
            lock_ = spin_lock_init(spin_lock_claim_unused(true));
        }

        // Static trampoline — the one callback the SDK knows about
        static void dispatch(uint gpio, uint32_t events) {
            GpioIrqManager& mgr = instance();
            // No lock here: reading a std::function from ISR context is safe as long
            // as we never resize the map during a potential concurrent dispatch.
            // For stricter safety, swap to a flat array (see note below).
            auto it = mgr.callbacks_.find(gpio);
            if (it != mgr.callbacks_.end() && it->second) {
                it->second(gpio, events);
            }
        }

        spin_lock_t*                          lock_      = nullptr;
        std::unordered_map<uint, Callback>    callbacks_;
        bool                                  installed_ = false;
    };
}