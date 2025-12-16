#pragma once

// ============================================================================
// Auto-generated web pages - DO NOT EDIT DIRECTLY
// Edit files in webui/ and run scripts/generate_web_pages.py
// ============================================================================

#include <Arduino.h>

const char PAGE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Motor Controller</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    :root {
      --bg: #1a1a2e;
      --card: #16213e;
      --accent: #0f3460;
      --highlight: #e94560;
      --text: #eee;
      --muted: #888;
      --success: #4ade80;
      --warning: #fbbf24;
      --danger: #ef4444;
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      padding: 1rem;
    }
    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem;
      background: var(--card);
      border-radius: 8px;
      margin-bottom: 1rem;
    }
    header h1 { font-size: 1.5rem; }
    .status-bar {
      display: flex;
      gap: 1rem;
      align-items: center;
    }
    .status-indicator {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      font-size: 0.875rem;
    }
    .dot {
      width: 10px;
      height: 10px;
      border-radius: 50%;
      background: var(--success);
    }
    .dot.warning { background: var(--warning); }
    .dot.danger { background: var(--danger); }
    nav {
      display: flex;
      gap: 0.5rem;
      margin-bottom: 1rem;
    }
    nav a {
      padding: 0.75rem 1.5rem;
      background: var(--card);
      color: var(--text);
      text-decoration: none;
      border-radius: 8px;
      transition: background 0.2s;
    }
    nav a:hover, nav a.active { background: var(--accent); }
    .estop-btn {
      background: var(--danger);
      color: white;
      border: none;
      padding: 0.75rem 2rem;
      font-size: 1rem;
      font-weight: bold;
      border-radius: 8px;
      cursor: pointer;
      text-transform: uppercase;
      transition: transform 0.1s, box-shadow 0.1s;
    }
    .estop-btn:hover { transform: scale(1.05); box-shadow: 0 0 20px rgba(239,68,68,0.5); }
    .estop-btn:active { transform: scale(0.98); }
    .estop-btn.active { animation: pulse 1s infinite; }
    @keyframes pulse {
      0%, 100% { box-shadow: 0 0 10px rgba(239,68,68,0.5); }
      50% { box-shadow: 0 0 30px rgba(239,68,68,0.8); }
    }
    .motor-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
      gap: 1rem;
    }
    .motor-card {
      background: var(--card);
      border-radius: 12px;
      padding: 1.25rem;
      border: 2px solid transparent;
      transition: border-color 0.2s;
    }
    .motor-card.active { border-color: var(--highlight); }
    .motor-card.estop { border-color: var(--danger); opacity: 0.7; }
    .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 1rem;
    }
    .card-header h3 { font-size: 1.1rem; }
    .motor-type-badge {
      padding: 0.25rem 0.75rem;
      background: var(--accent);
      border-radius: 20px;
      font-size: 0.75rem;
      text-transform: uppercase;
    }
    .motor-type-badge.none { background: var(--muted); }
    .config-form, .control-form {
      display: flex;
      flex-direction: column;
      gap: 0.75rem;
    }
    label {
      font-size: 0.875rem;
      color: var(--muted);
      margin-bottom: 0.25rem;
      display: block;
    }
    select, input[type="number"], input[type="text"] {
      width: 100%;
      padding: 0.625rem;
      background: var(--bg);
      border: 1px solid var(--accent);
      border-radius: 6px;
      color: var(--text);
      font-size: 0.9rem;
    }
    select:focus, input:focus {
      outline: none;
      border-color: var(--highlight);
    }
    .pin-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 0.5rem;
    }
    .pin-input { display: flex; flex-direction: column; }
    .btn-row {
      display: flex;
      gap: 0.5rem;
      margin-top: 0.5rem;
    }
    button {
      padding: 0.625rem 1rem;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      font-size: 0.875rem;
      transition: background 0.2s, transform 0.1s;
    }
    button:active { transform: scale(0.97); }
    .btn-primary { background: var(--highlight); color: white; }
    .btn-primary:hover { background: #d63d56; }
    .btn-secondary { background: var(--accent); color: var(--text); }
    .btn-secondary:hover { background: #1a4980; }
    .btn-danger { background: var(--danger); color: white; }
    .btn-stop { background: var(--warning); color: #000; font-weight: bold; }
    .slider-group {
      margin: 0.75rem 0;
    }
    .slider-header {
      display: flex;
      justify-content: space-between;
      margin-bottom: 0.5rem;
    }
    input[type="range"] {
      width: 100%;
      height: 8px;
      -webkit-appearance: none;
      background: var(--bg);
      border-radius: 4px;
      outline: none;
    }
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 20px;
      height: 20px;
      background: var(--highlight);
      border-radius: 50%;
      cursor: pointer;
    }
    .motor-status {
      margin-top: 1rem;
      padding-top: 1rem;
      border-top: 1px solid var(--accent);
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 0.5rem;
      font-size: 0.8rem;
    }
    .status-item {
      display: flex;
      justify-content: space-between;
    }
    .status-label { color: var(--muted); }
    .status-value { font-family: monospace; }
    .quick-controls {
      display: flex;
      gap: 0.5rem;
      flex-wrap: wrap;
      margin-top: 0.75rem;
    }
    .quick-btn {
      flex: 1;
      min-width: 60px;
      padding: 0.5rem;
      font-size: 0.8rem;
    }
    .system-info {
      background: var(--card);
      border-radius: 8px;
      padding: 1rem;
      margin-top: 1rem;
      display: flex;
      justify-content: space-around;
      flex-wrap: wrap;
      gap: 1rem;
    }
    .info-item {
      text-align: center;
    }
    .info-value {
      font-size: 1.5rem;
      font-weight: bold;
      color: var(--highlight);
    }
    .info-label {
      font-size: 0.75rem;
      color: var(--muted);
      text-transform: uppercase;
    }
    .hidden { display: none !important; }
    .toast {
      position: fixed;
      bottom: 2rem;
      left: 50%;
      transform: translateX(-50%);
      background: var(--card);
      padding: 1rem 2rem;
      border-radius: 8px;
      border-left: 4px solid var(--success);
      animation: slideUp 0.3s;
    }
    .toast.error { border-color: var(--danger); }
    @keyframes slideUp {
      from { opacity: 0; transform: translate(-50%, 20px); }
      to { opacity: 1; transform: translate(-50%, 0); }
    }
  </style>
</head>
<body>
  <header>
    <h1>Motor Controller</h1>
    <div class="status-bar">
      <div class="status-indicator">
        <span class="dot" id="connDot"></span>
        <span id="connStatus">Connected</span>
      </div>
      <button class="estop-btn" id="estopBtn" onclick="toggleEstop()">E-STOP</button>
    </div>
  </header>

  <nav>
    <a href="/" class="active">Dashboard</a>
    <a href="/presets">Presets</a>
    <a href="/settings">Settings</a>
  </nav>

  <div class="motor-grid" id="motorGrid">
    <!-- Motor cards generated by JS -->
  </div>

  <div class="system-info">
    <div class="info-item">
      <div class="info-value" id="heapFree">--</div>
      <div class="info-label">Heap Free</div>
    </div>
    <div class="info-item">
      <div class="info-value" id="uptime">--</div>
      <div class="info-label">Uptime</div>
    </div>
    <div class="info-item">
      <div class="info-value" id="version">--</div>
      <div class="info-label">Version</div>
    </div>
  </div>

  <div id="toast" class="toast hidden"></div>

  <script>
    const MOTOR_TYPES = {
      0: { name: 'None', pins: [] },
      1: { name: 'DC L298N', pins: ['in1', 'in2', 'ena'] },
      2: { name: 'DC L9110S', pins: ['ia', 'ib'] },
      3: { name: 'Servo', pins: ['signal'] },
      4: { name: 'Stepper A4988', pins: ['step', 'dir', 'enable'] },
      5: { name: 'Stepper DRV8825', pins: ['step', 'dir', 'enable'] },
      6: { name: 'Stepper ULN2003', pins: ['in1', 'in2', 'in3', 'in4'] }
    };

    const DEFAULT_PINS = [
      { in1: 13, in2: 12, ena: 14, ia: 13, ib: 12, signal: 13, step: 13, dir: 12, enable: 14 },
      { in1: 27, in2: 26, ena: 25, ia: 27, ib: 26, signal: 27, step: 27, dir: 26, enable: 25 },
      { in1: 33, in2: 32, ena: 35, ia: 33, ib: 32, signal: 33, step: 33, dir: 32, enable: 35 },
      { in1: 19, in2: 18, ena: 5, ia: 19, ib: 18, signal: 19, step: 19, dir: 18, enable: 5 }
    ];

    let motorState = [{}, {}, {}, {}];
    let estopActive = false;
    let pollInterval;

    function init() {
      renderMotorCards();
      startPolling();
    }

    function renderMotorCards() {
      const grid = document.getElementById('motorGrid');
      grid.innerHTML = '';
      for (let i = 0; i < 4; i++) {
        grid.appendChild(createMotorCard(i));
      }
    }

    function createMotorCard(slot) {
      const card = document.createElement('div');
      card.className = 'motor-card';
      card.id = `motor-${slot}`;
      card.innerHTML = `
        <div class="card-header">
          <h3>Motor Slot ${slot}</h3>
          <span class="motor-type-badge none" id="badge-${slot}">Unconfigured</span>
        </div>
        <div class="config-form" id="config-${slot}">
          <div>
            <label>Motor Type</label>
            <select id="type-${slot}" onchange="onTypeChange(${slot})">
              ${Object.entries(MOTOR_TYPES).map(([k,v]) =>
                `<option value="${k}">${v.name}</option>`).join('')}
            </select>
          </div>
          <div class="pin-grid" id="pins-${slot}"></div>
          <div class="btn-row">
            <button class="btn-primary" onclick="configureMotor(${slot})">Configure</button>
            <button class="btn-danger" onclick="removeMotor(${slot})">Remove</button>
          </div>
        </div>
        <div class="control-form hidden" id="control-${slot}">
          <div id="controls-${slot}"></div>
          <div class="quick-controls">
            <button class="quick-btn btn-stop" onclick="stopMotor(${slot})">STOP</button>
            <button class="quick-btn btn-secondary" onclick="brakeMotor(${slot})">Brake</button>
            <button class="quick-btn btn-secondary" onclick="coastMotor(${slot})">Coast</button>
          </div>
          <div class="motor-status" id="status-${slot}"></div>
        </div>
      `;
      return card;
    }

    function onTypeChange(slot) {
      const type = parseInt(document.getElementById(`type-${slot}`).value);
      const pinsDiv = document.getElementById(`pins-${slot}`);
      const pins = MOTOR_TYPES[type].pins;

      if (pins.length === 0) {
        pinsDiv.innerHTML = '';
        return;
      }

      pinsDiv.innerHTML = pins.map(pin => `
        <div class="pin-input">
          <label>${pin.toUpperCase()}</label>
          <input type="number" id="pin-${slot}-${pin}"
                 value="${DEFAULT_PINS[slot][pin] || 0}" min="0" max="39">
        </div>
      `).join('');
    }

    function renderControls(slot, type) {
      const div = document.getElementById(`controls-${slot}`);
      switch(type) {
        case 1: case 2: // DC motors
          div.innerHTML = `
            <div class="slider-group">
              <div class="slider-header">
                <span>Speed</span>
                <span id="speed-val-${slot}">0%</span>
              </div>
              <input type="range" id="speed-${slot}" min="-100" max="100" value="0"
                     oninput="updateSpeedLabel(${slot})" onchange="setSpeed(${slot})">
            </div>
          `;
          break;
        case 3: // Servo
          div.innerHTML = `
            <div class="slider-group">
              <div class="slider-header">
                <span>Angle</span>
                <span id="angle-val-${slot}">90&deg;</span>
              </div>
              <input type="range" id="angle-${slot}" min="0" max="180" value="90"
                     oninput="updateAngleLabel(${slot})" onchange="setAngle(${slot})">
            </div>
            <div class="btn-row">
              <button class="btn-secondary" onclick="setAngle(${slot}, 0)">0&deg;</button>
              <button class="btn-secondary" onclick="setAngle(${slot}, 90)">90&deg;</button>
              <button class="btn-secondary" onclick="setAngle(${slot}, 180)">180&deg;</button>
            </div>
          `;
          break;
        case 4: case 5: case 6: // Steppers
          div.innerHTML = `
            <div class="slider-group">
              <div class="slider-header">
                <span>Speed (steps/s)</span>
                <span id="stepper-speed-val-${slot}">500</span>
              </div>
              <input type="range" id="stepper-speed-${slot}" min="10" max="2000" value="500"
                     oninput="updateStepperSpeedLabel(${slot})">
            </div>
            <div style="margin-bottom: 0.5rem;">
              <label>Target Position</label>
              <input type="number" id="pos-${slot}" value="0">
            </div>
            <div class="btn-row">
              <button class="btn-primary" onclick="moveToPosition(${slot})">Go To</button>
              <button class="btn-secondary" onclick="moveRelative(${slot}, -100)">-100</button>
              <button class="btn-secondary" onclick="moveRelative(${slot}, 100)">+100</button>
              <button class="btn-secondary" onclick="homeMotor(${slot})">Home</button>
            </div>
          `;
          break;
      }
    }

    function updateSpeedLabel(slot) {
      const val = document.getElementById(`speed-${slot}`).value;
      document.getElementById(`speed-val-${slot}`).textContent = `${val}%`;
    }

    function updateAngleLabel(slot) {
      const val = document.getElementById(`angle-${slot}`).value;
      document.getElementById(`angle-val-${slot}`).innerHTML = `${val}&deg;`;
    }

    function updateStepperSpeedLabel(slot) {
      const val = document.getElementById(`stepper-speed-${slot}`).value;
      document.getElementById(`stepper-speed-val-${slot}`).textContent = val;
    }

    async function configureMotor(slot) {
      const type = parseInt(document.getElementById(`type-${slot}`).value);
      if (type === 0) return toast('Select a motor type', true);

      const pins = {};
      MOTOR_TYPES[type].pins.forEach(pin => {
        pins[pin] = parseInt(document.getElementById(`pin-${slot}-${pin}`).value);
      });

      try {
        const res = await fetch(`/api/motors/${slot}/configure`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ type, pins })
        });
        const data = await res.json();
        if (data.success) {
          toast(`Motor ${slot} configured as ${MOTOR_TYPES[type].name}`);
          updateMotorCard(slot, type);
        } else {
          toast(data.error || 'Configuration failed', true);
        }
      } catch (e) {
        toast('Connection error', true);
      }
    }

    function updateMotorCard(slot, type) {
      const card = document.getElementById(`motor-${slot}`);
      const badge = document.getElementById(`badge-${slot}`);
      const configForm = document.getElementById(`config-${slot}`);
      const controlForm = document.getElementById(`control-${slot}`);

      if (type > 0) {
        card.classList.add('active');
        badge.className = 'motor-type-badge';
        badge.textContent = MOTOR_TYPES[type].name;
        configForm.classList.add('hidden');
        controlForm.classList.remove('hidden');
        renderControls(slot, type);
      } else {
        card.classList.remove('active');
        badge.className = 'motor-type-badge none';
        badge.textContent = 'Unconfigured';
        configForm.classList.remove('hidden');
        controlForm.classList.add('hidden');
      }
    }

    async function removeMotor(slot) {
      try {
        await fetch(`/api/motors/${slot}/remove`, { method: 'POST' });
        updateMotorCard(slot, 0);
        toast(`Motor ${slot} removed`);
      } catch (e) {
        toast('Error removing motor', true);
      }
    }

    async function sendCommand(slot, command, value = 0, duration = 0) {
      if (estopActive && command !== 'stop') {
        return toast('E-Stop active!', true);
      }
      try {
        const res = await fetch(`/api/motors/${slot}/control`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ command, value, duration })
        });
        const data = await res.json();
        if (!data.success) toast(data.error || 'Command failed', true);
      } catch (e) {
        toast('Connection error', true);
      }
    }

    function setSpeed(slot) {
      const speed = parseInt(document.getElementById(`speed-${slot}`).value);
      sendCommand(slot, 'speed', speed * 2.55); // Scale to 0-255
    }

    function setAngle(slot, angle) {
      if (angle === undefined) {
        angle = parseInt(document.getElementById(`angle-${slot}`).value);
      } else {
        document.getElementById(`angle-${slot}`).value = angle;
        updateAngleLabel(slot);
      }
      sendCommand(slot, 'angle', angle);
    }

    function moveToPosition(slot) {
      const pos = parseInt(document.getElementById(`pos-${slot}`).value);
      sendCommand(slot, 'position', pos);
    }

    function moveRelative(slot, steps) {
      sendCommand(slot, 'relative', steps);
    }

    function stopMotor(slot) { sendCommand(slot, 'stop'); }
    function brakeMotor(slot) { sendCommand(slot, 'brake'); }
    function coastMotor(slot) { sendCommand(slot, 'coast'); }
    function homeMotor(slot) { sendCommand(slot, 'home'); }

    async function toggleEstop() {
      const btn = document.getElementById('estopBtn');
      try {
        if (estopActive) {
          await fetch('/api/system/estop/reset', { method: 'POST' });
        } else {
          await fetch('/api/system/estop', { method: 'POST' });
        }
      } catch (e) {
        toast('Connection error', true);
      }
    }

    function updateEstopState(active) {
      estopActive = active;
      const btn = document.getElementById('estopBtn');
      btn.classList.toggle('active', active);
      btn.textContent = active ? 'RESET E-STOP' : 'E-STOP';

      document.querySelectorAll('.motor-card').forEach(card => {
        card.classList.toggle('estop', active);
      });
    }

    async function pollStatus() {
      try {
        const res = await fetch('/api/status');
        const data = await res.json();

        // Update connection status
        document.getElementById('connDot').className = 'dot';
        document.getElementById('connStatus').textContent = 'Connected';

        // Update system info
        document.getElementById('heapFree').textContent =
          Math.round(data.system.heap / 1024) + ' KB';
        document.getElementById('uptime').textContent =
          formatUptime(data.system.uptime);
        document.getElementById('version').textContent = data.system.version;

        // Update E-stop state
        if (data.safety) {
          updateEstopState(data.safety.estopActive);
        }

        // Update motor states
        if (data.motors && data.motors.slots) {
          data.motors.slots.forEach((slot, i) => {
            motorState[i] = slot;
            if (slot.configured) {
              updateMotorCard(i, slot.type);
              updateMotorStatus(i, slot);
            }
          });
        }
      } catch (e) {
        document.getElementById('connDot').className = 'dot danger';
        document.getElementById('connStatus').textContent = 'Disconnected';
      }
    }

    function updateMotorStatus(slot, data) {
      const statusDiv = document.getElementById(`status-${slot}`);
      if (!statusDiv) return;

      statusDiv.innerHTML = `
        <div class="status-item">
          <span class="status-label">Position</span>
          <span class="status-value">${data.position || 0}</span>
        </div>
        <div class="status-item">
          <span class="status-label">Speed</span>
          <span class="status-value">${data.speed || 0}</span>
        </div>
        <div class="status-item">
          <span class="status-label">Moving</span>
          <span class="status-value">${data.moving ? 'Yes' : 'No'}</span>
        </div>
        <div class="status-item">
          <span class="status-label">Enabled</span>
          <span class="status-value">${data.enabled ? 'Yes' : 'No'}</span>
        </div>
      `;
    }

    function formatUptime(seconds) {
      const h = Math.floor(seconds / 3600);
      const m = Math.floor((seconds % 3600) / 60);
      const s = seconds % 60;
      return `${h}h ${m}m ${s}s`;
    }

    function startPolling() {
      pollStatus();
      pollInterval = setInterval(pollStatus, 500);
    }

    function toast(msg, isError = false) {
      const t = document.getElementById('toast');
      t.textContent = msg;
      t.className = 'toast' + (isError ? ' error' : '');
      setTimeout(() => t.classList.add('hidden'), 3000);
    }

    // Initialize on load
    document.addEventListener('DOMContentLoaded', init);
  </script>
</body>
</html>

)rawliteral";

const char PAGE_SETTINGS[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Settings - Motor Controller</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    :root {
      --bg: #1a1a2e;
      --card: #16213e;
      --accent: #0f3460;
      --highlight: #e94560;
      --text: #eee;
      --muted: #888;
      --success: #4ade80;
      --warning: #fbbf24;
      --danger: #ef4444;
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      padding: 1rem;
    }
    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem;
      background: var(--card);
      border-radius: 8px;
      margin-bottom: 1rem;
    }
    header h1 { font-size: 1.5rem; }
    nav {
      display: flex;
      gap: 0.5rem;
      margin-bottom: 1rem;
    }
    nav a {
      padding: 0.75rem 1.5rem;
      background: var(--card);
      color: var(--text);
      text-decoration: none;
      border-radius: 8px;
      transition: background 0.2s;
    }
    nav a:hover, nav a.active { background: var(--accent); }
    .settings-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
      gap: 1rem;
    }
    .panel {
      background: var(--card);
      border-radius: 12px;
      padding: 1.25rem;
    }
    .panel h2 {
      font-size: 1.1rem;
      margin-bottom: 1rem;
      padding-bottom: 0.5rem;
      border-bottom: 1px solid var(--accent);
      display: flex;
      align-items: center;
      gap: 0.5rem;
    }
    .panel-icon { font-size: 1.25rem; }
    label {
      font-size: 0.875rem;
      color: var(--muted);
      margin-bottom: 0.25rem;
      display: block;
    }
    input, select {
      width: 100%;
      padding: 0.625rem;
      background: var(--bg);
      border: 1px solid var(--accent);
      border-radius: 6px;
      color: var(--text);
      font-size: 0.9rem;
      margin-bottom: 0.75rem;
    }
    input:focus, select:focus {
      outline: none;
      border-color: var(--highlight);
    }
    input[type="file"] {
      padding: 0.5rem;
    }
    button {
      padding: 0.625rem 1rem;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      font-size: 0.875rem;
      transition: background 0.2s;
    }
    .btn-primary { background: var(--highlight); color: white; }
    .btn-primary:hover { background: #d63d56; }
    .btn-secondary { background: var(--accent); color: var(--text); }
    .btn-secondary:hover { background: #1a4980; }
    .btn-danger { background: var(--danger); color: white; }
    .btn-warning { background: var(--warning); color: #000; }
    .btn-row {
      display: flex;
      gap: 0.5rem;
      margin-top: 0.5rem;
    }
    .info-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 0.75rem;
    }
    .info-item {
      padding: 0.75rem;
      background: var(--bg);
      border-radius: 8px;
    }
    .info-label {
      font-size: 0.75rem;
      color: var(--muted);
      text-transform: uppercase;
      margin-bottom: 0.25rem;
    }
    .info-value {
      font-size: 1rem;
      font-family: monospace;
    }
    .info-value.highlight { color: var(--highlight); }
    .progress-bar {
      height: 8px;
      background: var(--bg);
      border-radius: 4px;
      overflow: hidden;
      margin: 1rem 0;
    }
    .progress-fill {
      height: 100%;
      background: var(--highlight);
      transition: width 0.3s;
    }
    .upload-status {
      text-align: center;
      padding: 1rem;
      display: none;
    }
    .upload-status.active { display: block; }
    .upload-status .icon {
      font-size: 3rem;
      margin-bottom: 0.5rem;
    }
    .wifi-mode {
      display: flex;
      gap: 0.5rem;
      margin-bottom: 1rem;
    }
    .wifi-mode button {
      flex: 1;
      padding: 1rem;
      background: var(--bg);
      border: 2px solid var(--accent);
    }
    .wifi-mode button.active {
      border-color: var(--highlight);
      background: var(--accent);
    }
    .status-badge {
      display: inline-block;
      padding: 0.25rem 0.75rem;
      border-radius: 20px;
      font-size: 0.75rem;
      text-transform: uppercase;
    }
    .status-badge.connected { background: var(--success); color: #000; }
    .status-badge.disconnected { background: var(--danger); color: white; }
    .status-badge.ap { background: var(--warning); color: #000; }
    .danger-zone {
      border: 2px solid var(--danger);
      margin-top: 1rem;
    }
    .danger-zone h2 { color: var(--danger); }
    .hidden { display: none !important; }
    .toast {
      position: fixed;
      bottom: 2rem;
      left: 50%;
      transform: translateX(-50%);
      background: var(--card);
      padding: 1rem 2rem;
      border-radius: 8px;
      border-left: 4px solid var(--success);
    }
    .toast.error { border-color: var(--danger); }
  </style>
</head>
<body>
  <header>
    <h1>Settings</h1>
  </header>

  <nav>
    <a href="/">Dashboard</a>
    <a href="/presets">Presets</a>
    <a href="/settings" class="active">Settings</a>
  </nav>

  <div class="settings-grid">
    <!-- System Info -->
    <div class="panel">
      <h2><span class="panel-icon">&#9881;</span> System Information</h2>
      <div class="info-grid">
        <div class="info-item">
          <div class="info-label">Firmware</div>
          <div class="info-value" id="firmware">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Version</div>
          <div class="info-value highlight" id="version">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Chip</div>
          <div class="info-value" id="chip">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">CPU Freq</div>
          <div class="info-value" id="cpuFreq">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Heap Free</div>
          <div class="info-value" id="heapFree">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Heap Min</div>
          <div class="info-value" id="heapMin">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Flash Size</div>
          <div class="info-value" id="flashSize">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Uptime</div>
          <div class="info-value" id="uptime">-</div>
        </div>
      </div>
    </div>

    <!-- WiFi Settings -->
    <div class="panel">
      <h2><span class="panel-icon">&#128246;</span> WiFi Configuration</h2>

      <div class="info-item" style="margin-bottom: 1rem;">
        <div class="info-label">Current Mode</div>
        <div class="info-value">
          <span class="status-badge" id="wifiModeBadge">-</span>
          <span id="wifiSSID" style="margin-left: 0.5rem;">-</span>
        </div>
      </div>

      <div class="info-grid" style="margin-bottom: 1rem;">
        <div class="info-item">
          <div class="info-label">IP Address</div>
          <div class="info-value" id="wifiIP">-</div>
        </div>
        <div class="info-item">
          <div class="info-label">Signal</div>
          <div class="info-value" id="wifiRSSI">-</div>
        </div>
      </div>

      <h3 style="font-size: 0.9rem; margin-bottom: 0.75rem;">Change WiFi</h3>

      <div class="wifi-mode">
        <button id="modeAP" onclick="setWifiMode(true)">Access Point</button>
        <button id="modeSTA" onclick="setWifiMode(false)">Join Network</button>
      </div>

      <div>
        <label>SSID</label>
        <input type="text" id="wifiNewSSID" placeholder="Network name">
      </div>
      <div>
        <label>Password</label>
        <input type="password" id="wifiNewPassword" placeholder="Password (min 8 chars)">
      </div>

      <button class="btn-primary" onclick="saveWifi()">Save &amp; Reboot</button>
    </div>

    <!-- OTA Update -->
    <div class="panel">
      <h2><span class="panel-icon">&#8635;</span> Firmware Update (OTA)</h2>

      <div id="otaForm">
        <p style="color: var(--muted); margin-bottom: 1rem; font-size: 0.875rem;">
          Upload a new firmware binary (.bin) to update the device over WiFi.
        </p>

        <div>
          <label>Firmware File</label>
          <input type="file" id="firmwareFile" accept=".bin">
        </div>

        <button class="btn-warning" onclick="uploadFirmware()">Upload Firmware</button>
      </div>

      <div class="upload-status" id="uploadStatus">
        <div class="icon" id="uploadIcon">&#8635;</div>
        <div id="uploadMessage">Uploading...</div>
        <div class="progress-bar">
          <div class="progress-fill" id="uploadProgress" style="width: 0%"></div>
        </div>
      </div>
    </div>

    <!-- Motor Config -->
    <div class="panel">
      <h2><span class="panel-icon">&#9881;</span> Motor Configuration</h2>

      <div class="info-item" style="margin-bottom: 1rem;">
        <div class="info-label">Configured Motors</div>
        <div class="info-value highlight" id="motorCount">0 / 4</div>
      </div>

      <div class="btn-row">
        <button class="btn-primary" onclick="saveMotorConfig()">Save Config to Flash</button>
        <button class="btn-secondary" onclick="location.href='/'">Configure Motors</button>
      </div>
    </div>

    <!-- Danger Zone -->
    <div class="panel danger-zone">
      <h2><span class="panel-icon">&#9888;</span> Danger Zone</h2>

      <p style="color: var(--muted); margin-bottom: 1rem; font-size: 0.875rem;">
        These actions cannot be undone. Use with caution.
      </p>

      <div class="btn-row">
        <button class="btn-danger" onclick="factoryReset()">Factory Reset</button>
        <button class="btn-warning" onclick="rebootDevice()">Reboot Device</button>
      </div>
    </div>
  </div>

  <div id="toast" class="toast hidden"></div>

  <script>
    let apMode = true;

    function init() {
      loadSystemInfo();
      loadWifiInfo();
      setInterval(loadSystemInfo, 5000);
    }

    async function loadSystemInfo() {
      try {
        const res = await fetch('/api/system/info');
        const data = await res.json();

        document.getElementById('firmware').textContent = data.firmware || 'Motor Controller';
        document.getElementById('version').textContent = data.version || '-';
        document.getElementById('chip').textContent = data.chip || '-';
        document.getElementById('cpuFreq').textContent = (data.cpuFreq || 0) + ' MHz';
        document.getElementById('heapFree').textContent = formatBytes(data.heapFree);
        document.getElementById('heapMin').textContent = formatBytes(data.heapMin);
        document.getElementById('flashSize').textContent = formatBytes(data.flashSize);
        document.getElementById('uptime').textContent = data.uptimeStr || '-';
        document.getElementById('motorCount').textContent =
          (data.configuredMotors || 0) + ' / 4';
      } catch (e) {
        console.error('Failed to load system info', e);
      }
    }

    async function loadWifiInfo() {
      try {
        const res = await fetch('/api/system/wifi');
        const data = await res.json();

        const badge = document.getElementById('wifiModeBadge');
        if (data.mode === 'AP') {
          badge.className = 'status-badge ap';
          badge.textContent = 'AP Mode';
          document.getElementById('wifiRSSI').textContent = data.stations + ' clients';
        } else {
          badge.className = data.connected ? 'status-badge connected' : 'status-badge disconnected';
          badge.textContent = data.connected ? 'Connected' : 'Disconnected';
          document.getElementById('wifiRSSI').textContent = (data.rssi || 0) + ' dBm';
        }

        document.getElementById('wifiSSID').textContent = data.ssid || '-';
        document.getElementById('wifiIP').textContent = data.ip || '-';

        apMode = data.mode === 'AP';
        updateWifiModeButtons();
      } catch (e) {
        console.error('Failed to load wifi info', e);
      }
    }

    function setWifiMode(isAP) {
      apMode = isAP;
      updateWifiModeButtons();
    }

    function updateWifiModeButtons() {
      document.getElementById('modeAP').classList.toggle('active', apMode);
      document.getElementById('modeSTA').classList.toggle('active', !apMode);
    }

    async function saveWifi() {
      const ssid = document.getElementById('wifiNewSSID').value.trim();
      const password = document.getElementById('wifiNewPassword').value;

      if (!ssid) return toast('Enter SSID', true);
      if (password.length > 0 && password.length < 8) {
        return toast('Password must be at least 8 characters', true);
      }

      if (!confirm('Save WiFi settings and reboot?')) return;

      try {
        const res = await fetch('/api/system/wifi', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ ssid, password, apMode })
        });
        const data = await res.json();
        if (data.success) {
          toast('WiFi saved, rebooting...');
        } else {
          toast(data.error || 'Failed to save', true);
        }
      } catch (e) {
        toast('Connection error', true);
      }
    }

    async function uploadFirmware() {
      const fileInput = document.getElementById('firmwareFile');
      if (!fileInput.files.length) {
        return toast('Select a firmware file', true);
      }

      const file = fileInput.files[0];
      if (!file.name.endsWith('.bin')) {
        return toast('File must be a .bin firmware', true);
      }

      if (!confirm('Upload new firmware? Device will reboot after update.')) return;

      const formData = new FormData();
      formData.append('firmware', file);

      document.getElementById('otaForm').classList.add('hidden');
      const status = document.getElementById('uploadStatus');
      status.classList.add('active');

      try {
        const xhr = new XMLHttpRequest();
        xhr.open('POST', '/api/system/ota', true);

        xhr.upload.onprogress = (e) => {
          if (e.lengthComputable) {
            const pct = (e.loaded / e.total * 100).toFixed(0);
            document.getElementById('uploadProgress').style.width = pct + '%';
            document.getElementById('uploadMessage').textContent = `Uploading... ${pct}%`;
          }
        };

        xhr.onload = () => {
          if (xhr.status === 200) {
            document.getElementById('uploadIcon').innerHTML = '&#10004;';
            document.getElementById('uploadMessage').textContent = 'Update complete! Rebooting...';
            document.getElementById('uploadProgress').style.width = '100%';
          } else {
            showUploadError('Upload failed');
          }
        };

        xhr.onerror = () => showUploadError('Connection error');
        xhr.send(formData);

      } catch (e) {
        showUploadError('Upload error');
      }
    }

    function showUploadError(msg) {
      document.getElementById('uploadIcon').innerHTML = '&#10006;';
      document.getElementById('uploadMessage').textContent = msg;
      document.getElementById('uploadProgress').style.background = 'var(--danger)';

      setTimeout(() => {
        document.getElementById('otaForm').classList.remove('hidden');
        document.getElementById('uploadStatus').classList.remove('active');
        document.getElementById('uploadProgress').style.width = '0%';
        document.getElementById('uploadProgress').style.background = '';
      }, 3000);
    }

    async function saveMotorConfig() {
      try {
        await fetch('/api/motors/save-config', { method: 'POST' });
        toast('Configuration saved to flash');
      } catch (e) {
        toast('Failed to save', true);
      }
    }

    async function factoryReset() {
      if (!confirm('This will erase ALL settings including motor configurations and presets. Continue?')) return;
      if (!confirm('Are you REALLY sure? This cannot be undone!')) return;

      try {
        await fetch('/api/system/factory-reset', { method: 'POST' });
        toast('Factory reset complete, rebooting...');
      } catch (e) {
        toast('Failed to reset', true);
      }
    }

    async function rebootDevice() {
      if (!confirm('Reboot the device?')) return;

      try {
        await fetch('/api/system/reboot', { method: 'POST' });
        toast('Rebooting...');
      } catch (e) {
        toast('Failed to reboot', true);
      }
    }

    function formatBytes(bytes) {
      if (!bytes) return '-';
      if (bytes < 1024) return bytes + ' B';
      if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB';
      return (bytes / (1024 * 1024)).toFixed(1) + ' MB';
    }

    function toast(msg, isError = false) {
      const t = document.getElementById('toast');
      t.textContent = msg;
      t.className = 'toast' + (isError ? ' error' : '');
      setTimeout(() => t.classList.add('hidden'), 3000);
    }

    document.addEventListener('DOMContentLoaded', init);
  </script>
</body>
</html>

)rawliteral";

const char PAGE_PRESETS[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Presets - Motor Controller</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    :root {
      --bg: #1a1a2e;
      --card: #16213e;
      --accent: #0f3460;
      --highlight: #e94560;
      --text: #eee;
      --muted: #888;
      --success: #4ade80;
      --warning: #fbbf24;
      --danger: #ef4444;
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      padding: 1rem;
    }
    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem;
      background: var(--card);
      border-radius: 8px;
      margin-bottom: 1rem;
    }
    header h1 { font-size: 1.5rem; }
    nav {
      display: flex;
      gap: 0.5rem;
      margin-bottom: 1rem;
    }
    nav a {
      padding: 0.75rem 1.5rem;
      background: var(--card);
      color: var(--text);
      text-decoration: none;
      border-radius: 8px;
      transition: background 0.2s;
    }
    nav a:hover, nav a.active { background: var(--accent); }
    .content {
      display: grid;
      grid-template-columns: 1fr 2fr;
      gap: 1rem;
    }
    @media (max-width: 900px) {
      .content { grid-template-columns: 1fr; }
    }
    .panel {
      background: var(--card);
      border-radius: 12px;
      padding: 1.25rem;
    }
    .panel h2 {
      font-size: 1.1rem;
      margin-bottom: 1rem;
      padding-bottom: 0.5rem;
      border-bottom: 1px solid var(--accent);
    }
    .preset-list {
      display: flex;
      flex-direction: column;
      gap: 0.5rem;
    }
    .preset-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 0.75rem;
      background: var(--bg);
      border-radius: 8px;
      cursor: pointer;
      transition: background 0.2s;
    }
    .preset-item:hover { background: var(--accent); }
    .preset-item.active { border: 2px solid var(--highlight); }
    .preset-item.playing { border: 2px solid var(--success); animation: glow 1s infinite; }
    @keyframes glow {
      0%, 100% { box-shadow: 0 0 5px rgba(74,222,128,0.3); }
      50% { box-shadow: 0 0 15px rgba(74,222,128,0.6); }
    }
    .preset-name { font-weight: 500; }
    .preset-steps { font-size: 0.8rem; color: var(--muted); }
    .preset-actions { display: flex; gap: 0.25rem; }
    .icon-btn {
      width: 32px;
      height: 32px;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 1rem;
      background: var(--accent);
      color: var(--text);
    }
    .icon-btn:hover { background: var(--highlight); }
    .icon-btn.danger:hover { background: var(--danger); }
    button {
      padding: 0.625rem 1rem;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      font-size: 0.875rem;
      transition: background 0.2s;
    }
    .btn-primary { background: var(--highlight); color: white; }
    .btn-primary:hover { background: #d63d56; }
    .btn-secondary { background: var(--accent); color: var(--text); }
    .btn-secondary:hover { background: #1a4980; }
    .btn-danger { background: var(--danger); color: white; }
    .btn-success { background: var(--success); color: #000; }
    .btn-warning { background: var(--warning); color: #000; }
    label {
      font-size: 0.875rem;
      color: var(--muted);
      margin-bottom: 0.25rem;
      display: block;
    }
    input, select {
      width: 100%;
      padding: 0.625rem;
      background: var(--bg);
      border: 1px solid var(--accent);
      border-radius: 6px;
      color: var(--text);
      font-size: 0.9rem;
      margin-bottom: 0.75rem;
    }
    input:focus, select:focus {
      outline: none;
      border-color: var(--highlight);
    }
    .form-row {
      display: flex;
      gap: 0.5rem;
    }
    .form-row > * { flex: 1; }
    .step-list {
      max-height: 400px;
      overflow-y: auto;
      margin: 1rem 0;
    }
    .step-item {
      display: grid;
      grid-template-columns: 40px 1fr 80px 80px 40px;
      gap: 0.5rem;
      align-items: center;
      padding: 0.5rem;
      background: var(--bg);
      border-radius: 6px;
      margin-bottom: 0.5rem;
    }
    .step-num {
      font-weight: bold;
      color: var(--highlight);
      text-align: center;
    }
    .step-item select, .step-item input {
      margin-bottom: 0;
      padding: 0.4rem;
    }
    .btn-row {
      display: flex;
      gap: 0.5rem;
      margin-top: 1rem;
    }
    .recording-indicator {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      padding: 0.75rem 1rem;
      background: var(--danger);
      border-radius: 8px;
      margin-bottom: 1rem;
      animation: pulse 1s infinite;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.7; }
    }
    .recording-dot {
      width: 12px;
      height: 12px;
      background: white;
      border-radius: 50%;
    }
    .playback-controls {
      display: flex;
      gap: 0.5rem;
      padding: 1rem;
      background: var(--accent);
      border-radius: 8px;
      margin-bottom: 1rem;
      align-items: center;
    }
    .playback-status {
      flex: 1;
      font-size: 0.9rem;
    }
    .hidden { display: none !important; }
    .toast {
      position: fixed;
      bottom: 2rem;
      left: 50%;
      transform: translateX(-50%);
      background: var(--card);
      padding: 1rem 2rem;
      border-radius: 8px;
      border-left: 4px solid var(--success);
    }
    .toast.error { border-color: var(--danger); }
    .empty-state {
      text-align: center;
      padding: 2rem;
      color: var(--muted);
    }
    .checkbox-row {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      margin-bottom: 0.75rem;
    }
    .checkbox-row input[type="checkbox"] {
      width: auto;
      margin: 0;
    }
  </style>
</head>
<body>
  <header>
    <h1>Motion Presets</h1>
  </header>

  <nav>
    <a href="/">Dashboard</a>
    <a href="/presets" class="active">Presets</a>
    <a href="/settings">Settings</a>
  </nav>

  <div class="playback-controls hidden" id="playbackControls">
    <div class="playback-status">
      <strong>Now Playing:</strong> <span id="playingName">-</span>
      <br><small>Step <span id="playingStep">0</span></small>
    </div>
    <button class="btn-danger" onclick="stopPlayback()">Stop</button>
  </div>

  <div class="recording-indicator hidden" id="recordingIndicator">
    <span class="recording-dot"></span>
    <span>Recording: <strong id="recordingName">-</strong></span>
    <button class="btn-secondary" onclick="recordStep()" style="margin-left: auto;">Capture Step</button>
    <button class="btn-warning" onclick="stopRecording()">Stop</button>
  </div>

  <div class="content">
    <div class="panel">
      <h2>Saved Presets</h2>
      <div class="preset-list" id="presetList">
        <div class="empty-state">No presets yet</div>
      </div>
      <div class="btn-row">
        <button class="btn-primary" onclick="showNewPreset()">New Preset</button>
        <button class="btn-secondary" onclick="startRecording()">Record</button>
      </div>
    </div>

    <div class="panel">
      <h2 id="editorTitle">Preset Editor</h2>

      <div id="editorForm">
        <div class="form-row">
          <div>
            <label>Preset Name</label>
            <input type="text" id="presetName" placeholder="My Preset">
          </div>
        </div>

        <div class="checkbox-row">
          <input type="checkbox" id="presetLoop">
          <label for="presetLoop" style="margin: 0;">Loop playback</label>
        </div>

        <h3 style="font-size: 0.9rem; margin: 1rem 0 0.5rem;">Sequence Steps</h3>
        <div class="step-list" id="stepList">
          <!-- Steps added dynamically -->
        </div>

        <button class="btn-secondary" onclick="addStep()" style="width: 100%;">+ Add Step</button>

        <div class="btn-row">
          <button class="btn-primary" onclick="savePreset()">Save Preset</button>
          <button class="btn-secondary" onclick="clearEditor()">Clear</button>
        </div>
      </div>
    </div>
  </div>

  <div id="toast" class="toast hidden"></div>

  <script>
    let presets = [];
    let currentPreset = null;
    let steps = [];
    let isRecording = false;
    let isPlaying = false;
    let pollInterval;

    const COMMANDS = [
      { value: 'stop', label: 'Stop' },
      { value: 'speed', label: 'Set Speed' },
      { value: 'position', label: 'Go To Position' },
      { value: 'angle', label: 'Set Angle' },
      { value: 'relative', label: 'Move Relative' },
      { value: 'delay', label: 'Delay (ms)' }
    ];

    function init() {
      loadPresets();
      startPolling();
    }

    async function loadPresets() {
      try {
        const res = await fetch('/api/presets');
        const data = await res.json();
        presets = data.presets || [];
        renderPresetList();
        updatePlaybackState(data);
      } catch (e) {
        console.error('Failed to load presets', e);
      }
    }

    function renderPresetList() {
      const list = document.getElementById('presetList');
      if (presets.length === 0) {
        list.innerHTML = '<div class="empty-state">No presets yet</div>';
        return;
      }

      list.innerHTML = presets.map(p => `
        <div class="preset-item ${currentPreset === p.name ? 'active' : ''}"
             onclick="selectPreset('${p.name}')">
          <div>
            <div class="preset-name">${p.name}</div>
            <div class="preset-steps">${p.stepCount || 0} steps${p.loop ? ' (loop)' : ''}</div>
          </div>
          <div class="preset-actions" onclick="event.stopPropagation()">
            <button class="icon-btn" onclick="playPreset('${p.name}')" title="Play">&#9658;</button>
            <button class="icon-btn danger" onclick="deletePreset('${p.name}')" title="Delete">&#10005;</button>
          </div>
        </div>
      `).join('');
    }

    async function selectPreset(name) {
      currentPreset = name;
      renderPresetList();

      try {
        const res = await fetch(`/api/presets/${name}`);
        const data = await res.json();

        document.getElementById('presetName').value = data.name;
        document.getElementById('presetLoop').checked = data.loop || false;
        steps = data.steps || [];
        renderSteps();
        document.getElementById('editorTitle').textContent = `Editing: ${name}`;
      } catch (e) {
        toast('Failed to load preset', true);
      }
    }

    function showNewPreset() {
      currentPreset = null;
      document.getElementById('presetName').value = '';
      document.getElementById('presetLoop').checked = false;
      steps = [];
      renderSteps();
      document.getElementById('editorTitle').textContent = 'New Preset';
      renderPresetList();
    }

    function clearEditor() {
      showNewPreset();
    }

    function renderSteps() {
      const list = document.getElementById('stepList');
      if (steps.length === 0) {
        list.innerHTML = '<div class="empty-state">No steps - add some!</div>';
        return;
      }

      list.innerHTML = steps.map((step, i) => `
        <div class="step-item">
          <span class="step-num">${i + 1}</span>
          <select onchange="updateStep(${i}, 'slot', this.value)">
            <option value="0" ${step.slot === 0 ? 'selected' : ''}>Motor 0</option>
            <option value="1" ${step.slot === 1 ? 'selected' : ''}>Motor 1</option>
            <option value="2" ${step.slot === 2 ? 'selected' : ''}>Motor 2</option>
            <option value="3" ${step.slot === 3 ? 'selected' : ''}>Motor 3</option>
            <option value="-1" ${step.slot === -1 ? 'selected' : ''}>All / Delay</option>
          </select>
          <select onchange="updateStep(${i}, 'command', this.value)">
            ${COMMANDS.map(c => `<option value="${c.value}" ${step.command === c.value ? 'selected' : ''}>${c.label}</option>`).join('')}
          </select>
          <input type="number" value="${step.value || 0}"
                 onchange="updateStep(${i}, 'value', this.value)" placeholder="Value">
          <button class="icon-btn danger" onclick="removeStep(${i})">&#10005;</button>
        </div>
      `).join('');
    }

    function addStep() {
      steps.push({ slot: 0, command: 'speed', value: 0 });
      renderSteps();
    }

    function removeStep(index) {
      steps.splice(index, 1);
      renderSteps();
    }

    function updateStep(index, field, value) {
      if (field === 'slot' || field === 'value') {
        steps[index][field] = parseInt(value);
      } else {
        steps[index][field] = value;
      }
    }

    async function savePreset() {
      const name = document.getElementById('presetName').value.trim();
      if (!name) return toast('Enter a preset name', true);
      if (steps.length === 0) return toast('Add at least one step', true);

      const preset = {
        name,
        loop: document.getElementById('presetLoop').checked,
        steps
      };

      try {
        const res = await fetch('/api/presets', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(preset)
        });
        const data = await res.json();
        if (data.success) {
          toast('Preset saved');
          loadPresets();
          currentPreset = name;
        } else {
          toast(data.error || 'Failed to save', true);
        }
      } catch (e) {
        toast('Connection error', true);
      }
    }

    async function deletePreset(name) {
      if (!confirm(`Delete preset "${name}"?`)) return;

      try {
        await fetch(`/api/presets/${name}`, { method: 'DELETE' });
        toast('Preset deleted');
        if (currentPreset === name) showNewPreset();
        loadPresets();
      } catch (e) {
        toast('Failed to delete', true);
      }
    }

    async function playPreset(name) {
      try {
        await fetch(`/api/presets/${name}/play`, { method: 'POST' });
        toast(`Playing: ${name}`);
      } catch (e) {
        toast('Failed to start playback', true);
      }
    }

    async function stopPlayback() {
      try {
        await fetch('/api/presets/stop', { method: 'POST' });
        toast('Playback stopped');
      } catch (e) {
        toast('Failed to stop', true);
      }
    }

    async function startRecording() {
      const name = prompt('Recording name:', 'Recording ' + Date.now());
      if (!name) return;

      try {
        await fetch('/api/presets/record/start', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ name })
        });
        isRecording = true;
        document.getElementById('recordingIndicator').classList.remove('hidden');
        document.getElementById('recordingName').textContent = name;
        toast('Recording started - use dashboard to move motors, then capture steps');
      } catch (e) {
        toast('Failed to start recording', true);
      }
    }

    async function recordStep() {
      try {
        await fetch('/api/presets/record/step', { method: 'POST' });
        toast('Step recorded');
      } catch (e) {
        toast('Failed to record step', true);
      }
    }

    async function stopRecording() {
      try {
        await fetch('/api/presets/record/stop', { method: 'POST' });
        isRecording = false;
        document.getElementById('recordingIndicator').classList.add('hidden');
        toast('Recording saved');
        loadPresets();
      } catch (e) {
        toast('Failed to stop recording', true);
      }
    }

    function updatePlaybackState(data) {
      const controls = document.getElementById('playbackControls');
      const recording = document.getElementById('recordingIndicator');

      if (data.playing) {
        controls.classList.remove('hidden');
        document.getElementById('playingName').textContent = data.playingPreset || '-';
        document.getElementById('playingStep').textContent = data.currentStep || 0;
      } else {
        controls.classList.add('hidden');
      }

      if (data.recording) {
        isRecording = true;
        recording.classList.remove('hidden');
        document.getElementById('recordingName').textContent = data.recordingName || '-';
      } else {
        isRecording = false;
        recording.classList.add('hidden');
      }
    }

    async function pollStatus() {
      try {
        const res = await fetch('/api/presets/status');
        const data = await res.json();
        updatePlaybackState(data);
      } catch (e) {
        // Ignore poll errors
      }
    }

    function startPolling() {
      pollInterval = setInterval(pollStatus, 1000);
    }

    function toast(msg, isError = false) {
      const t = document.getElementById('toast');
      t.textContent = msg;
      t.className = 'toast' + (isError ? ' error' : '');
      setTimeout(() => t.classList.add('hidden'), 3000);
    }

    document.addEventListener('DOMContentLoaded', init);
  </script>
</body>
</html>

)rawliteral";


// Page lengths for Content-Length header
#define PAGE_INDEX_LEN (sizeof(PAGE_INDEX) - 1)
#define PAGE_SETTINGS_LEN (sizeof(PAGE_SETTINGS) - 1)
#define PAGE_PRESETS_LEN (sizeof(PAGE_PRESETS) - 1)
