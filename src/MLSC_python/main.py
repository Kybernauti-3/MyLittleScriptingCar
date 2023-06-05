from fastapi import FastAPI, Request, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, StreamingResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from random import randint
import cv2
from time import sleep
from serial import Serial

app = FastAPI()

video = cv2.VideoCapture(0)
templates = Jinja2Templates(directory="templates")
app.mount("/static", StaticFiles(directory="static"), name="static")
ser = Serial("/dev/serial0", 115200, timeout=1)
ser.reset_input_buffer()


@app.get("/", response_class=HTMLResponse)
async def index(request: Request):
    return templates.TemplateResponse('index.html', {"request": request, "video": "video_feed"})


def gen():
    """Video streaming generator function."""
    while True:
        success, image = video.read()
        ret, jpeg = cv2.imencode('.jpg', image)
        yield b'--frame\r\nContent-Type: image/jpeg\r\n\r\n' + jpeg.tobytes() + b'\r\n'


def motor_speed(direction: int, velocity: int):
    if velocity > 0:
        lm = velocity
        rm = -velocity
        lm += direction / 2
        rm += direction / 2
    elif velocity < 0:
        lm = velocity
        rm = -velocity
        lm -= direction / 2
        rm -= direction / 2
    else:
        lm = direction / 2
        rm = direction / 2

    ser.write(bytes("slv" + str(int(lm)) + "\n", "utf-8"))
    ser.write(bytes("srv" + str(int(rm)) + "\n", "utf-8"))
    print("slv" + str(int(lm)) + "\n")
    print("srv" + str(int(rm)) + "\n")


@app.get('/video_feed', response_class=StreamingResponse)
async def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    sleep(0.05)
    return StreamingResponse(gen(), media_type='multipart/x-mixed-replace; boundary=frame')


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    # ser.open()
    while True:
        data = await websocket.receive_text()
        com, *vals = data.split(":")

        if com == "gu":
            ser.write(b"gu\n")
            distance = ser.readline().decode('utf-8').rstrip()
            await websocket.send_text(f"{distance}")
        elif com == "su":
            ser.write(bytes("su" + vals[0] + "\n", "utf-8"))
        elif com == "sn":
            ser.write(bytes("sln" + vals[0] + "\n", "utf-8"))
            ser.write(bytes("srn" + vals[0] + "\n", "utf-8"))
        elif com == "sv":
            direction, velocity = vals[0].split(",")
            motor_speed(int(direction), int(velocity))
        else:
            print(data)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app=app, host="0.0.0.0", port=8010)
