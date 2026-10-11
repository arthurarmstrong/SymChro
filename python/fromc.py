from pysymchro import symchro, nanoseconds
from datetime import datetime, timedelta
import asyncio
from time import perf_counter, sleep

s = symchro(100)

work = timedelta(microseconds=40)

last_sleep = timedelta(seconds=0)


def main():
    while True:
        s.start_work()
        sleep(work.total_seconds())
        s.end_work()

        global last_sleep

        req_sleep: timedelta = s.get_sleep_required_ns(
            timedelta(seconds=0.2),
            37,
        )

        sleep(req_sleep.total_seconds())

        last_sleep = req_sleep


if __name__ == "__main__":
    main()
