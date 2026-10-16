import asyncio
from dataclasses import dataclass
from datetime import timedelta
from time import perf_counter


from nicegui import app, ui, background_tasks, run
from pysymchro import symchro

sim_dt = timedelta(seconds=0.2)


async def update():

    config = app.storage.client

    while True:
        sync.check_point()

        t_sleep = sync.get_sleep_required_ns(sim_dt, int(config["ref_rate"]))

        config["current_ratio"] = sync.current_ratio
        config["wall_time"] = perf_counter() - config["start_time"]
        config["sim_elapsed"] += sim_dt.total_seconds()

        await asyncio.sleep(t_sleep.total_seconds())


async def page():

    config = app.storage.client

    config["ref_rate"] = 1.0
    config["wall_time"] = 0.0
    config["sim_elapsed"] = 0.0
    config["last_twall"] = perf_counter()
    config["start_time"] = perf_counter()  # when we started
    config["current_ratio"] = 10.0  # current sim time to wall time ratio
    config["last_sleep"] = 0.0  # how long to sleep for
    config["t_work"] = 0.1  # how long the last cycle took

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

        _sleeping_for = ui.label().bind_text_from(
            config,
            "t_sleep",
            backward=lambda t: f"Sleeping: {config['t_sleep'] * 1000:.0f}ms",
        )

        ui.slider(
            min=1.0, max=100.0, step=1, value=config["current_ratio"]
        ).bind_value_to(config, "ref_rate")


async def start_worker():
    await run.cpu_bound(update)


sync = symchro()

app.on_connect(update)
ui.run(page)
