from invoke import task
import serial
import time


@task
def build(ctx, prog=False, mon=False):
    ctx.run("make -j16")
    if prog:
        program(ctx)
    if mon:
        monitor(ctx)


@task
def clean(ctx):
    ctx.run("make clean")


@task
def program(ctx, mon=False):
    ctx.run("JLinkExe program.jlink")
    if mon:
        monitor(ctx)


@task
def monitor(ctx, port="", baud=921600):
    if not port:
        with open(".monitor_port", "r") as f:
            port = f.readlines()[0]
    else:
        with open(".monitor_port", "w") as f:
            f.write(port)
    ser = serial.Serial(port=port, baudrate=baud, timeout=0.01)
    while True:
        data = ser.readline()
        if data:
            print(data.decode(), end="", flush=True)
        time.sleep(0.01)


@task
def create_flash_image(ctx):
    return


@task
def load_flash_image(ctx):
    return
