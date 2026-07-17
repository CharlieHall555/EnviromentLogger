const API_BASE_URL = import.meta.env.VITE_API_BASE_URL?.trim() ?? "";

export class ApiError extends Error {
  status: number;
  path: string;
  url: string;

  constructor(path: string, url: string, status: number) {
    super(`GET ${url} failed with status ${status}`);
    this.name = "ApiError";
    this.status = status;
    this.path = path;
    this.url = url;
  }
}

function buildApiUrl(path: string): string {
  if (!API_BASE_URL) {
    return path;
  }

  const normalizedBaseUrl = API_BASE_URL.endsWith("/")
    ? API_BASE_URL.slice(0, -1)
    : API_BASE_URL;

  return `${normalizedBaseUrl}${path}`;
}

export async function apiGet<T>(path: string): Promise<T> {
  const url = buildApiUrl(path);
  const response = await fetch(url, {
    method: "GET",
    headers: {
      "Accept": "application/json",
    },
  });

  if (!response.ok) {
    throw new ApiError(path, url, response.status);
  }

  return response.json() as Promise<T>;
}