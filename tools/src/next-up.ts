// next-up.ts
// Read-only preview of what today's `daily-dsa` session would serve.
// Mirrors the selection logic in agents/daily-dsa.md step 1 — but this
// script only *reads* context/state.md and context/problem-bank.md, it
// never writes anything.
//
// Run from inside tools/ (`npm run next-up`, or `npx tsx src/next-up.ts`
// with your cwd set to tools/) — the relative paths below assume that cwd.
//
// The markdown parsing here is intentionally basic and dependency-free
// (no npm packages beyond typescript/tsx) — it's good enough for our own
// tables, not meant to handle arbitrary markdown. This whole package is a
// learning vehicle, so clarity beats cleverness throughout.

import { readFileSync } from 'node:fs';

type Row = Record<string, string>;

// Parses a GitHub-flavored markdown table into an array of row objects,
// keyed by the header cells. Any line that isn't a "| ... |" row is
// ignored, so headings and prose around the table are skipped for free.
function parseTable(markdown: string): Row[] {
  const tableLines = markdown.split('\n').filter((line) => line.trim().startsWith('|'));
  if (tableLines.length < 2) return [];

  const header = splitRow(tableLines[0]);
  const dataLines = tableLines.slice(2); // line 0 = header, line 1 = "|---|---|" separator

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
  // "| a | b |" -> ["", " a ", " b ", ""] -> trim, then drop the empty ends
  const cells = line.split('|').map((c) => c.trim());
  return cells.slice(1, -1);
}

// Parses the "- key: value" bullet format used in state.md. Multi-line
// HTML comments (used there for inline explanations) don't start with
// "- ", so they're skipped automatically.
function parseState(markdown: string): Record<string, string> {
  const state: Record<string, string> = {};
  for (const rawLine of markdown.split('\n')) {
    const line = rawLine.trim();
    if (!line.startsWith('- ')) continue;
    const colonIdx = line.indexOf(':');
    if (colonIdx === -1) continue;
    const key = line.slice(2, colonIdx).trim();
    const value = line.slice(colonIdx + 1).trim();
    state[key] = value;
  }
  return state;
}

// "[cses-1068, dp-a]" -> ["cses-1068", "dp-a"]; "[]" or undefined -> []
function parseIdList(raw: string | undefined): string[] {
  if (!raw) return [];
  const inner = raw.trim().replace(/^\[/, '').replace(/\]$/, '').trim();
  return inner ? inner.split(',').map((s) => s.trim()).filter(Boolean) : [];
}

function describe(row: Row): string {
  return [
    `${row.id}  (${row.source}, ${row.difficulty})`,
    `  ${row.title}`,
    `  ${row.url}`,
    `  topic: ${row.topic}`,
  ].join('\n');
}

function main() {
  const stateRaw = readFileSync('../context/state.md', 'utf-8');
  const bankRaw = readFileSync('../context/problem-bank.md', 'utf-8');

  const state = parseState(stateRaw);
  const bank = parseTable(bankRaw);
  const today = new Date().toISOString().slice(0, 10); // YYYY-MM-DD

  // Step 1a (agents/daily-dsa.md): a due spaced-repetition review wins.
  const pendingIds = parseIdList(state['pending_review_ids']);
  const dueRow = bank.find(
    (row) => pendingIds.includes(row.id) && row.next_review !== '' && row.next_review <= today,
  );

  if (dueRow) {
    console.log('Next up (spaced-repetition review is due):\n');
    console.log(describe(dueRow));
    return;
  }

  // Step 1b: selection_mode "random-combined" — uniform random pick from
  // every still-unsolved problem across all three sources, no rotation.
  const todoRows = bank.filter((row) => row.status === 'todo');
  if (todoRows.length === 0) {
    console.log('No status:todo problems left in problem-bank.md — nothing to serve.');
    return;
  }

  const pick = todoRows[Math.floor(Math.random() * todoRows.length)];
  console.log(`Next up (random pick from ${todoRows.length} unsolved problems):\n`);
  console.log(describe(pick));
}

main();
