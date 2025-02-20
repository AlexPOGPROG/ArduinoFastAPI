from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
import serial_reader  # Import the serial reader script

app = FastAPI()

# Mount a static directory for serving HTML/CSS/JS files
app.mount("/static", StaticFiles(directory="static"), name="static")

# Endpoint to serve the HTML page
@app.get("/", response_class=HTMLResponse)
async def read_root():
    with open("static/index.html", "r") as file:
        return HTMLResponse(content=file.read())

# Endpoint to fetch sensor data
@app.get("/sensor-data")
async def get_sensor_data():
    sensor_data = serial_reader.read_sensor_data()
    return sensor_data

# Run the FastAPI server
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)