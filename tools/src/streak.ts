// streak.ts
// Reads context/progress.md directly and recomputes the current daily
// streak from scratch — a sanity check against state.md's cached
// streak_days, computed independently from the source of truth.
//
// Run from inside tools/ (`npm run streak`, or `npx tsx src/streak.ts`
// with your cwd set to tools/) — the relative path below assumes that cwd.

import { readFileSync } from 'node:fs';

type Row = Record<string, string>;

// Same small parser as next-up.ts, duplicated on purpose so each script
// stays readable on its own — this package is a learning vehicle, not a
// shared library.
function parseTable(markdown: string): Row[] {
  const tableLines = markdown.split('\n').filter((line) => line.trim().startsWith('|'));
  if (tableLines.length < 2) return [];

  const header = splitRow(tableLines[0]);
  const dataLines = tableLines.slice(2);

  return dataLines.map((line) => {
    const cells = splitRow(line);
    const row: Row = {};
    header.forEach((key, i) => {
      row[key] = cells[i] ?? '';
    });
    return row;
  });
}

function splitRow(line: string): string[] {
  const cells = line.split('|').map((c) => c.trim());
  return cells.slice(1, -1);
}

// Whole-day difference between two YYYY-MM-DD dates (later - earlier).
function daysBetween(laterISODate: string, earlierISODate: string): number {
  const later = new Date(`${laterISODate}T00:00:00Z`).getTime();
  const earlier = new Date(`${earlierISODate}T00:00:00Z`).getTime();
  return Math.round((later - earlier) / (1000 * 60 * 60 * 24));
}

function main() {
  const progressRaw = readFileSync('../context/progress.md', 'utf-8');
  const rows = parseTable(progressRaw);

  const uniqueDates = [...new Set(rows.map((r) => r.date).filter(Boolean))].sort().reverse();

  if (uniqueDates.length === 0) {
    console.log('No sessions logged yet. Streak: 0');
    return;
  }

  const today = new Date().toISOString().slice(0, 10);
  const mostRecent = uniqueDates[0];
  const gapFromToday = daysBetween(today, mostRecent);

  if (gapFromToday > 1) {
    console.log(`Streak broken — last session was ${mostRecent} (${gapFromToday} days ago). Streak: 0`);
    return;
  }

  let streak = 1;
  for (let i = 1; i < uniqueDates.length; i++) {
    if (daysBetween(uniqueDates[i - 1], uniqueDates[i]) === 1) {
      streak++;
    } else {
      break;
    }
  }

  console.log(`Current streak: ${streak} day(s). Last session: ${mostRecent}.`);
}

main();
