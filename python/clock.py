import asyncio
from collections import deque
from dataclasses import dataclass, field
from time import perf_counter

from nicegui import app, ui


@dataclass
class PID:
    ref: float = 1.0

    ref_err: float = 0.0

    integral: float = 0.0

    last_err_measured: float = 0.0

    P: float = 0.5
    I: float = 0.001
    D: float = 0.1

    def update(self, err_measured: float, dt):
        self.integral += err_measured
        derrdt = err_measured - self.last_err_measured
        self.last_err_measured = err_measured

        accel = -self.P * err_measured - self.I * self.integral - self.D * derrdt

        return accel  # * dt**2 / 2

    def reset(self):
        self.integral = 0.0


pid = PID()

sim_dt = 0.2


async def page():

    async def tick():
        config["sim_elapsed"] += sim_dt
        await asyncio.sleep(0.0)

    async def update():
        # We know how many sim seconds elapsed - sim_dt. How long did that really take?
        while True:
            _t = perf_counter()

            config["t_cycle"] = _t - config["last_twall"]
            config["last_twall"] = _t

            config["wall_time"] = _t - config["start_time"]

            config["t_last_ten_cycles"].append(config["t_cycle"])

            await tick()

            config["current_ratio"] = 10 * sim_dt / sum(config["t_last_ten_cycles"])

            sleep_should_be = sim_dt / config["ref_rate"] - (
                config["t_cycle"] - config["t_sleep"]
            )

            err_sleep_measured = 1 * (config["t_sleep"] - sleep_should_be)

            d_t_sleep = pid.update(err_sleep_measured, config["t_cycle"])

            config["t_sleep"] = max(0, config["t_sleep"] + d_t_sleep)
            t_sleep = config["t_sleep"]

            if t_sleep >= 0:
                await asyncio.sleep(t_sleep)
            else:
                await asyncio.sleep(0)

    config = app.storage.client

    config["ref_rate"] = 1.0
    config["wall_time"] = 0.0
    config["sim_elapsed"] = 0.0
    config["last_twall"] = perf_counter()
    config["start_time"] = perf_counter()  # when we started
    config["current_ratio"] = 1.0  # current sim time to wall time ratio
    config["t_sleep"] = 0.0  # how long to sleep for
    config["t_cycle"] = 0.1  # how long the last cycle took

    _dq = config["t_last_ten_cycles"] = deque(maxlen=10)

    with ui.card():
        ui.label("Speed up or slow down. It's your choice!")
        _wall_time_elapsed = ui.label().bind_text_from(
            config,
            "wall_time",
            backward=lambda t: f"Wall Time: {config['wall_time']:.2f}s",
        )
        _sim_time_elapsed = ui.label().bind_text_from(
            config,
            "sim_elapsed",
            backward=lambda t: f"Sim Time: {config['sim_elapsed']:.2f}s",
        )

        _desired_ratio = ui.label().bind_text_from(
            config, "ref_rate", backward=lambda f: f"Set Ratio: {f:.1f}"
        )

        _current_ratio = ui.label().bind_text_from(
            config,
            "current_ratio",
            backward=lambda t: f"Current Ratio: {t:.1f}",
        )

        sleeping_for = ui.label().bind_text_from(
            config,
            "t_sleep",
            backward=lambda t: f"Sleeping: {config['t_sleep'] * 1000:.0f}ms",
        )

        ui.slider(
            min=1.0, max=100.0, step=1, value=config["current_ratio"]
        ).bind_value_to(config, "ref_rate").on_value_change(pid.reset)

        ui.timer(0.03, update)

        await update()


ui.run(page)
