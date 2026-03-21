/* =============================================================
   C-Phase-1 · Centralized JavaScript
   All pages load this file.
   Add new shared behaviour here only.
============================================================= */

// ── TAB SWITCHING ──────────────────────────────────────────
// Used on all chapter pages.
// Called by onclick="show('sectionId', this)" on tab buttons.
function show(id, btn) {
  document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
  document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
  document.getElementById(id).classList.add('active');
  btn.classList.add('active');

  // persist active tab in URL hash so browser back/forward works
  history.replaceState(null, '', '#' + id);
}

// ── COPY BUTTON ────────────────────────────────────────────
// Used on terminal blocks.
// Called by onclick="copyCode(this)" on .copy-btn buttons.
function copyCode(btn) {
  const pre = btn.closest('.term').querySelector('pre');
  const raw = pre ? pre.innerText.trim() : '';
  navigator.clipboard.writeText(raw).then(() => {
    btn.textContent = 'copied!';
    btn.classList.add('copied');
    setTimeout(() => {
      btn.textContent = 'copy';
      btn.classList.remove('copied');
    }, 1800);
  });
}

// ── RESTORE TAB FROM URL HASH ──────────────────────────────
// If user visits a direct link like ch1.html#ex12,
// the correct tab opens automatically.
document.addEventListener('DOMContentLoaded', () => {
  const hash = window.location.hash.slice(1); // remove #
  if (hash) {
    const target = document.getElementById(hash);
    const btn = document.querySelector(`[onclick="show('${hash}', this)"]`);
    if (target && btn) {
      document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
      document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
      target.classList.add('active');
      btn.classList.add('active');
    }
  }
});

