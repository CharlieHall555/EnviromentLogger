import { apiGet } from "./apiClient";

export type ReadingSummary = {
    date: string;
    humidity_avg: number | null;
    humidity_max: number | null;
    humidity_min: number | null;
    temperature_avg: number | null;
    temperature_max: number | null;
    temperature_min: number | null;
    total_size: number;
};

export type WeeklySummary = {
    weekStart: string;
    weekEnd: string;
    days: ReadingSummary[]
};

function getMonday(dateString: string): Date {
    const date = new Date(dateString);
    const day = date.getDay(); // Sunday = 0, Monday = 1

    const diff = day === 0 ? -6 : 1 - day;

    const monday = new Date(date);
    monday.setDate(date.getDate() + diff);
    monday.setHours(0, 0, 0, 0);

    return monday;
}

function formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, "0");
    const day = String(date.getDate()).padStart(2, "0");
    return `${year}-${month}-${day}`;
}

function getWeekDates(dateString: string): string[] {
    const monday = getMonday(dateString);
    const dates: string[] = [];

    for (let i = 0; i < 7; i += 1) {
        const day = new Date(monday);
        day.setDate(monday.getDate() + i);
        dates.push(formatDate(day));
    }

    return dates;
}

export function groupSummariesByWeek(
    summaries: ReadingSummary[]
): WeeklySummary[] {
    const grouped = new Map<string, ReadingSummary[]>();

    for (const summary of summaries) {
        const monday = getMonday(summary.date);
        const weekStart = formatDate(monday);

        if (!grouped.has(weekStart)) {
            grouped.set(weekStart, []);
        }

        grouped.get(weekStart)!.push(summary);
    }

    return Array.from(grouped.entries()).map(([weekStart, days]) => {
        const start = new Date(weekStart);
        const end = new Date(start);
        end.setDate(start.getDate() + 6);

        return {
            weekStart,
            weekEnd: formatDate(end),
            days: days.sort((a, b) => a.date.localeCompare(b.date)),
        };
    });
}

type GetDayResponse = ReadingSummary | ReadingSummary[];

export async function fetchReadingSummaryByDate(
    date: string
): Promise<ReadingSummary | null> {
    try {
        const response = await apiGet<GetDayResponse>(
            `/data/get_day?date=${encodeURIComponent(date)}`
        );

        if (Array.isArray(response)) {
            return response[0] ?? null;
        }

        return response;
    } catch {
        return null;
    }
}

export async function fetchWeeklySummary(date: string): Promise<WeeklySummary> {
    const weekDates = getWeekDates(date);
    const readings = await Promise.all(
        weekDates.map((weekDate) => fetchReadingSummaryByDate(weekDate))
    );

    const weekStart = weekDates[0] ?? date;
    const weekEnd = weekDates[6] ?? date;

    return {
        weekStart,
        weekEnd,
        days: readings
            .filter((reading): reading is ReadingSummary => reading !== null)
            .sort((a, b) => a.date.localeCompare(b.date)),
    };
}