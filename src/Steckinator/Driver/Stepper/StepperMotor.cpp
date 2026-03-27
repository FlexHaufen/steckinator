/**
 * @file StepperMotor.cpp
 * @author flexhaufen
 * @brief Implementation of StepperMotor
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

// *** INCLUDES ***
#include "Steckinator/Driver/Stepper/StepperMotor.h"
#include "hardware/clocks.h"
#include <cmath>
#include <cassert>

// *** NAMESPACE ***
namespace Steckinator {

    // *** STATIC VARIABLES ***
    int StepperMotor::s_pio0Offset = -1;
    int StepperMotor::s_pio1Offset = -1;

    // *** PRIVATE FUNCTIONS ***

    uint StepperMotor::loadProgram(PIO pio) {
        // Load the PIO program onto the block only once; all SMs on the same
        // block share the same 32-instruction program memory.
        if (pio == pio0) {
            if (s_pio0Offset < 0) {
                s_pio0Offset = pio_add_program(pio0, &step_gen_program);
            }
            return static_cast<uint>(s_pio0Offset);
        } else {
            if (s_pio1Offset < 0) {
                s_pio1Offset = pio_add_program(pio1, &step_gen_program);
            }
            return static_cast<uint>(s_pio1Offset);
        }
    }

    void StepperMotor::applyClockDiv() {
        // 4 PIO cycles per step pulse (see stepper.pio)
        float div = static_cast<float>(clock_get_hz(clk_sys)) / (m_stepHz * 4.0f);
        pio_sm_set_clkdiv(m_pio, m_stateMachineIndex, div);
    }

    // ── Constructor / destructor ─────────────────────────────────────────────────

    StepperMotor::StepperMotor(PIO pio, uint sm, uint stepPin, uint dirPin, float stepHz)
    : m_pio(pio),
    m_stateMachineIndex(sm),
    m_pinStep(stepPin),
    m_pinDir(dirPin), 
    m_stepHz(stepHz) {
        
        // ── GPIO setup ────────────────────────────────────────────────────────
        gpio_init(m_pinDir);
        gpio_set_dir(m_pinDir, GPIO_OUT);
        gpio_set_function(m_pinDir, GPIO_FUNC_SIO);     // this is just in case the pin is used for something else by default
        gpio_put(m_pinDir, 1);


        // TODO (flex): Implement enable pin
        //gpio_init(m_pinEnable);   gpio_set_dir(m_pinEnable,  GPIO_OUT);  gpio_put(m_pinEnable,  1); // disabled

        // ── PIO setup ─────────────────────────────────────────────────────────
        uint offset = loadProgram(m_pio);
        step_gen_program_init(m_pio, m_stateMachineIndex, offset, m_pinStep, m_stepHz);

        // ── DMA setup ─────────────────────────────────────────────────────────
        m_dmaChannel = dma_claim_unused_channel(true);

        dma_channel_config cfg = dma_channel_get_default_config(m_dmaChannel);
        channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);
        channel_config_set_read_increment(&cfg,  true);   // advance through source buffer
        channel_config_set_write_increment(&cfg, false);  // always write to the same FIFO register
        channel_config_set_dreq(&cfg, pio_get_dreq(m_pio, m_stateMachineIndex, true)); // paced by PIO TX DREQ

        dma_channel_configure(
            m_dmaChannel,
            &cfg,
            &m_pio->txf[m_stateMachineIndex],  // destination: TX FIFO register (fixed)
            nullptr,           // source: set per transfer in moveDma()
            0,                 // count: set per transfer
            false              // do not start yet
        );
    }

    StepperMotor::~StepperMotor() {
        stop();
        dma_channel_unclaim(m_dmaChannel);
        pio_sm_set_enabled(m_pio, m_stateMachineIndex, false);
    }

    void StepperMotor::setSpeed(float stepHz) {
        m_stepHz = stepHz;
        applyClockDiv();
    }

    float StepperMotor::getSpeed() const {
        return m_stepHz;
    }

    void StepperMotor::setDirection(bool forward) {
        gpio_put(m_pinDir, forward);
    }

    void StepperMotor::move(int32_t steps) {
        if (steps == 0) {
            return;
        }

        setDirection((steps > 0));
        // pio_sm_put_blocking will stall only if the 4-word TX FIFO is full,
        // which in normal use is effectively instantaneous.
        pio_sm_put_blocking(m_pio, m_stateMachineIndex, static_cast<uint32_t>(std::abs(steps)));
    }

    void StepperMotor::moveDma(const uint32_t* buf, uint len) {
        if (buf == nullptr || len == 0) {
            return;
        }

        // Wait for any previous DMA transfer to complete before reconfiguring
        dma_channel_wait_for_finish_blocking(m_dmaChannel);

        dma_channel_set_read_addr(m_dmaChannel,  buf, false);
        dma_channel_set_trans_count(m_dmaChannel, len, true); // true = start immediately
    }

    void StepperMotor::waitUntilDone() const {
        dma_channel_wait_for_finish_blocking(m_dmaChannel);
        while (!pio_sm_is_tx_fifo_empty(m_pio, m_stateMachineIndex)) {
            tight_loop_contents();
        }
    }

    bool StepperMotor::isBusy() const {
        return (dma_channel_is_busy(m_dmaChannel) || !pio_sm_is_tx_fifo_empty(m_pio, m_stateMachineIndex));
    }

    void StepperMotor::stop() {
        // Abort DMA transfer
        dma_channel_abort(m_dmaChannel);
        // Drain the TX FIFO so the SM stops after the current step
        pio_sm_clear_fifos(m_pio, m_stateMachineIndex);
    }

}