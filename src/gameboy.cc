#include "gameboy.h"

Gameboy::Gameboy(std::vector<u8> cartridge_data, Options& options, std::vector<u8> save_data) :
    cartridge(get_cartridge(std::move(cartridge_data), std::move(save_data))),
    cpu(mmu, options),
    video(cpu, mmu),
    serial(options.print_serial),
    mmu(cartridge, cpu, video, input, serial, timer),
    timer(cpu),
    debugger(*this, options.debugger)
{
    log_set_level(get_log_level(options));
}

void Gameboy::button_pressed(GbButton button) {
    input.button_pressed(button);
}

void Gameboy::button_released(GbButton button) {
    input.button_released(button);
}

void Gameboy::debug_toggle_background() {
    video.debug_disable_background = !video.debug_disable_background;
}

void Gameboy::debug_toggle_sprites() {
    video.debug_disable_sprites = !video.debug_disable_sprites;
}

void Gameboy::debug_toggle_window() {
    video.debug_disable_window = !video.debug_disable_window;
}

void Gameboy::register_should_close_callback(const should_close_callback_t& _should_close_callback) {
    should_close_callback = _should_close_callback;
}

void Gameboy::register_vblank_callback(const vblank_callback_t& _vblank_callback) {
    video.register_vblank_callback(_vblank_callback);
}

void Gameboy::run(
    const should_close_callback_t& _should_close_callback,
    const vblank_callback_t& _vblank_callback
) {
    register_should_close_callback(_should_close_callback);
    register_vblank_callback(_vblank_callback);

    while (!should_close_callback()) {
        tick();
    }
}

void Gameboy::tick() {
    debugger.cycle();

    auto cycles = cpu.tick();
    elapsed_cycles += cycles.cycles;

    video.tick(cycles);
    timer.tick(cycles.cycles);
}

const std::vector<u8>& Gameboy::get_cartridge_ram() const {
    return cartridge->get_cartridge_ram();
}
