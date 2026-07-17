export type ReadingType = 
  | "temperature"
  | "humidity"
  | "pm1_0"
  | "pm2_5"
  | "pm10";

export type SensorReading = {
    id: number;
    temperature: number;
    humidity: number;
    measured_at: string;
    received_at: string;
    pm10: number,
    pm1_0: number,
    pm2_5: number
};

export function getBlankSensorReading(): SensorReading {
    return {
        "id" : -1,
        "temperature" : -1,
        "humidity" : -1,
        "measured_at" : "-1",
        "received_at" : "-1",
        "pm10": -1,
        "pm1_0" : -1,
        "pm2_5" : -1
    }

}


