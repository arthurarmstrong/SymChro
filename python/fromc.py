from pysymchro import symchro, nanoseconds
from datetime import datetime, timedelta
import asyncio
from time import perf_counter

s = symchro(100)

work = timedelta(microseconds=40)

last_sleep = timedelta(seconds=0)


async def main():
    while True:
        s.start_work()
        await asyncio.sleep(work.total_seconds())
        s.end_work()

        global last_sleep

        sleep: timedelta = s.get_sleep_required_ms(
            timedelta(microseconds=200e3),
            25,
        )

        last_sleep = sleep


if __name__ == "__main__":
    asyncio.run(main())
