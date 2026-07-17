import { apiGet } from "./apiClient";

export type SensorReading = {
  id: number;
  temperature: number | string | null;
  humidity: number | string | null;
  measured_at: string;
  received_at: string;
  pm10 : number,
  pm1_0 : number,
  pm2_5 : number
};

type ReadingPageResponse = {
  page: number;
  page_size: number;
  readings: SensorReading[];
};

export async function fetchReadingsPage(
  page: number,
  pageSize = 10
): Promise<ReadingPageResponse> {
  return apiGet<ReadingPageResponse>(
    `/data/page?page=${page}&page_size=${pageSize}`
  );
}

export async function fetchLatestReading(): Promise<SensorReading | null> {
  const response = await fetchReadingsPage(1, 1);

  return response.readings[0] ?? null;
}
