export type MetricValue = number | string | null | undefined;

export function toNumeric(value: MetricValue): number | null {
  if (value === null || value === undefined || value === "") {
    return null;
  }

  const numericValue = typeof value === "number" ? value : Number(value);
  return Number.isFinite(numericValue) ? numericValue : null;
}

export function formatMetric(
  value: MetricValue,
  unit = "",
  fractionDigits = 1
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

  if (numericValue < 24) {
    return "green-value";
  }

  if (numericValue < 30) {
    return "orange-value";
  }

  return "red-value";
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