export type MetricValue = number | string | null | undefined;

export function toNumeric(value: MetricValue): number | null {
  if (value === null || value === undefined || value === "") {
    return null;
  }

  const numericValue = typeof value === "number" ? value : Number(value);
  return Number.isFinite(numericValue) ? numericValue : null;
}

export function formatShortDateTime(value: string | null | undefined): string {
  if (!value) {
    return "-";
  }

  try {
    const date = new Date(value);
    if (isNaN(date.getTime())) {
      return value;
    }

    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');

    return `${year}-${month}-${day} ${hours}:${minutes}`;
  } catch {
    return value;
  }
}

export function formatMetric(
  value: MetricValue,
  unit = "",
  fractionDigits = 2
): string {
  const numericValue = toNumeric(value);

  if (numericValue === null) {
    return "-";
  }

  const formattedValue = numericValue.toFixed(fractionDigits);
  return unit ? `${formattedValue} ${unit}` : formattedValue;
}

export function getTemperatureClass(value: MetricValue): string {
  const numericValue = toNumeric(value);
  if (numericValue === null) {
    return "";
  }

  if (numericValue < 16) {
    return "blue-value";
  }

  if (numericValue < 27) {
    return "green-value";
  }

  return "red-value";
}

export function getHumidityClass(value: MetricValue): string {
  const numericValue = toNumeric(value);
  if (numericValue === null) {
    return "";
  }

  if (numericValue < 30) {
    return "orange-value"; // Too dry
  }

  if (numericValue <= 60) {
    return "green-value"; // Ideal range
  }

  if (numericValue <= 70) {
    return "orange-value"; // Getting humid
  }

  return "red-value"; // Too humid
}

export function getPM1Class(value: MetricValue): string {
  const numericValue = toNumeric(value);

  if (numericValue === null) {
    return "";
  }


  if (numericValue <= 10) {
    return "green-value";
  }

  if (numericValue <= 20) {
    return "orange-value";
  }

  return "red-value";
}

export function getPM25Class(value: MetricValue): string {
  const numericValue = toNumeric(value);

  if (numericValue === null) {
    return "";
  }


  if (numericValue <= 15) {
    return "green-value";
  }

  if (numericValue <= 35) {
    return "orange-value";
  }

  return "red-value";
}

export function getPM10Class(value: MetricValue): string {
  const numericValue = toNumeric(value);

  if (numericValue === null) {
    return "";
  }

  if (numericValue <= 45) {
    return "green-value";
  }

  if (numericValue <= 75) {
    return "orange-value";
  }

  return "red-value";
}