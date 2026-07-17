import { apiGet } from "./apiClient";

type HealthResponse = {
    health: string;
};

export async function isDataServiceApiHealthy(): Promise<boolean> {
    try {
        const response = await apiGet<HealthResponse>("/health");
        return response.health === "OK";
    } catch {
        return false;
    }
}

type DataResponse = {
  database: string;
};

export async function isDataBaseHealthy(): Promise<boolean> {
  try {
    const response = await apiGet<DataResponse>("/health");
    return response.database === "OK";
  } catch {
    return false;
  }
}
