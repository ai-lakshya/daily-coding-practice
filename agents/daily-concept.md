# Daily Concept Instructions

## 1. Pick the concept
Read context/state.md (category_rotation_order, category_cursor) and
context/concept-bank.md. Pick the next `status: todo` concept in the
category at category_cursor. Advance category_cursor for next session.

## 2. Open the conversation
Teach it, structured as:
- What it is (1-2 lines, no fluff)
- Why it matters / where it shows up in real systems — use the user's
  domain flavor from profile.md where it fits naturally, don't force it
- Core mechanics with a minimal code example in the relevant language
- Common misconceptions or pitfalls
- One small exercise or check-understanding question

This is the opening of a conversation, not a final answer — end by
inviting follow-up questions rather than moving on immediately.

## 3. Continue the conversation
Answer follow-up questions fully and go wherever they lead, even outside
the original 5-point structure above — depth over rushing to close. Do
not log anything or update state yet.

Do not end the session on your own judgment. A plain acknowledgment
("ok", "got it", "makes sense") is not a signal to close — keep going.
Only move to step 4 when the user explicitly signals they're done with
this concept (e.g. "I'm done", "let's move on", "next", "that covers it",
"log it").

## 4. Log it
Write concepts/<date>-<slug>.md with the full note, covering everything
actually discussed in the conversation — not just the opening from step
2. Structure:
- The original 5-part teaching content from step 2.
- A "## Follow-up Discussion" section documenting the specific questions
  the user asked and the answers given, in enough detail to be useful
  without re-reading the full conversation later. Skip this section only
  if the user asked nothing beyond the opening.

## 5. Update state
- concept-bank.md: status -> covered (or -> revisit if the user flagged
  confusion at any point in the conversation), last_covered -> today.
- progress.md: append one row; key_takeaway should reflect what the whole
  conversation actually covered, not just the opening.
