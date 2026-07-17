import { ApiError, apiGet } from "./apiClient";
import { type SensorReading } from "@/domain/sensorReading";

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

export type PageSizeReponse = {
    "n_pages": number,
    "page_size": number,
    "total_size": number,
};

export async function fetchReadingMaxPage(pageSize : number): Promise<PageSizeReponse> {
  return apiGet<PageSizeReponse>(
    `/data/size?page_size=${pageSize}`
  );
}

export type LatestReadings = {
  latest: SensorReading | null;
  previous: SensorReading | null;
};

export async function fetchLatestReadings(): Promise<LatestReadings> {
  try {
    const response = await fetchReadingsPage(1, 2);

    return {
      latest: response.readings[0] ?? null,
      previous: response.readings[1] ?? null
    };
  } catch (error) {
    if (error instanceof ApiError && error.status === 404) {
      return {
        latest: null,
        previous: null
      };
    }

    throw error;
  }
}
