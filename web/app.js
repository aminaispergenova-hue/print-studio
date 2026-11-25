// app.js - the web page
// it only shows data and sends what the user typed to C++
// all prices, discounts and checks are in the C++ class Studio (src/Studio.cpp)

const STORAGE_KEY = "printStudio";
const app = document.getElementById("app");

let studio = null;      // the C++ object
let stageNames = [];    // "Accepted", "In design", ...
let message = "";       // shown once on the next screen
let ordersFilter = { stage: "active", text: "" };

// ---------- products and their form fields ----------

const PAPER = {
  key: "paper", label: "Paper", type: "select",
  choices: [["glossy", "Glossy"], ["matte", "Matte (−100 ₸ per sheet)"], ["kraft", "Kraft (+100 ₸ per sheet)"]],
};
const SIDES = {
  key: "sides", label: "Printing", type: "select",
  choices: [["1", "Single-sided"], ["2", "Double-sided"]],
};
function sizes(list) {
  return { key: "size", label: "Size", type: "select", choices: list.map((s) => [s, s]) };
}

const PRODUCTS = {
  magazine: {
    label: "Personal magazine",
    fields: [
      { key: "title", label: "Issue title", type: "text" },
      { key: "pages", label: "Number of pages", type: "number" },
      sizes(["A5", "A4", "A3"]), PAPER,
    ],
  },
  newspaper: {
    label: "Newspaper",
    fields: [
      { key: "title", label: "Title or occasion", type: "text" },
      { key: "pages", label: "Number of pages", type: "number" },
      sizes(["A5", "A4", "A3"]), PAPER,
    ],
  },
  poster: {
    label: "Poster",
    fields: [{ key: "theme", label: "Poster theme", type: "text" }, sizes(["A4", "A3", "A2"]), PAPER],
  },
  invitation: {
    label: "Invitations",
    fields: [{ key: "eventName", label: "Event", type: "text" }, sizes(["A6", "A5", "A4", "A3"]), PAPER, SIDES],
  },
  card: {
    label: "Cards (guest names, menus, thank-you)",
    fields: [
      { key: "kind", label: "Card type", type: "select",
        choices: [["guest", "Guest name cards"], ["menu", "Menu cards"], ["thanks", "Thank-you cards"]] },
      { key: "guestNames", label: "Guest names (one per line)", type: "textarea", onlyForKind: "guest" },
      sizes(["A6", "A5", "A4", "A3"]), PAPER, SIDES,
    ],
  },
  envelope: {
    label: "Envelopes",
    fields: [{ key: "note", label: "Note (colour, which invitations)", type: "text" }],
  },
};

// ---------- start ----------

// every Studio method returns JSON text, here it becomes a JS object
function cpp(method, ...args) {
  return JSON.parse(studio[method](...args));
}

function saveToBrowser() {
  try {
    localStorage.setItem(STORAGE_KEY, studio.exportData());
  } catch (e) {
    console.log("could not save", e);
  }
}

async function loadFile(path) {
  const response = await fetch(path, { cache: "no-cache" });
  if (!response.ok) throw new Error("Could not load " + path);
  return response.text();
}

async function loadDemo() {
  cpp("loadData", await loadFile("data/demo.json"));
  saveToBrowser();
}

async function start() {
  try {
    const Module = await createStudioModule();  // loads studio.wasm
    studio = new Module.Studio();

    const result = cpp("loadTariffs", await loadFile("data/tariffs.json"));
    if (result.error) throw new Error(result.error);
    stageNames = cpp("stageNames");

    // first visit -> demo data, next visits -> what was saved in this browser
    let saved = null;
    try { saved = localStorage.getItem(STORAGE_KEY); } catch (e) {}
    if (saved === null || cpp("loadData", saved).error) {
      await loadDemo();
    }

    setupFooter();
    window.addEventListener("hashchange", showPage);
    showPage();
  } catch (e) {
    app.innerHTML = `<div class="error-box">Error: ${esc(e.message)}</div>`;
  }
}

// ---------- small helpers ----------

// protects the page from html inside names
function esc(value) {
  return String(value ?? "")
    .replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
}

function money(n) {
  return Number(n).toLocaleString("en-US") + " ₸";
}

function showDate(iso) {
  if (!iso) return "—";
  const [y, m, d] = iso.split("-");
  return `${d}.${m}.${y}`;
}

function today() {
  const d = new Date();
  return d.getFullYear() + "-" + String(d.getMonth() + 1).padStart(2, "0") + "-" + String(d.getDate()).padStart(2, "0");
}

function daysLeft(iso) {
  return Math.round((new Date(iso) - new Date(today())) / 86400000);
}

function badge(stage, name) {
  return `<span class="badge stage-${stage}">${esc(name)}</span>`;
}

function takeMessage() {
  const html = message ? `<div class="notice">${esc(message)}</div>` : "";
  message = "";
  return html;
}

function priceTable(p) {
  return `
    <table>
      <tr><td>Design (once)</td><td class="money">${money(p.design)}</td></tr>
      <tr><td>Printing, 1 copy</td><td class="money">${money(p.printOne)}</td></tr>
      <tr><td>Quantity</td><td class="money">${p.quantity} pcs</td></tr>
      <tr><td>Printing before discount</td><td class="money">${money(p.printAll)}</td></tr>
      <tr class="discount"><td>Printing discount ${p.discountPercent}%</td><td class="money">−${money(p.discount)}</td></tr>
      <tr class="total"><td>Item total</td><td class="money">${money(p.total)}</td></tr>
    </table>`;
}

// ---------- pages ----------
// #/customers, #/customer/5, #/draft, #/order/1001, #/orders

function showPage() {
  const [page, idText] = location.hash.replace("#/", "").split("/");
  const id = Number(idText);

  document.querySelectorAll("[data-nav]").forEach((link) => {
    const current = page === "orders" ? "orders" : "customers";
    link.classList.toggle("active", link.dataset.nav === current);
  });

  if (page === "customer") showCustomer(id);
  else if (page === "draft") showDraft();
  else if (page === "order") showOrder(id);
  else if (page === "orders") showOrders();
  else showCustomers();
  window.scrollTo(0, 0);
}

function go(hash) {
  if (location.hash === hash) showPage();
  else location.hash = hash;
}

// ---------- page 1: customers ----------

function showCustomers() {
  app.innerHTML = `
    <h1>Customers</h1>
    <p class="subtitle">Find a customer by name or add a new one.</p>
    ${takeMessage()}
    <div class="card">
      <div class="row">
        <input id="search" class="grow" style="width:auto" placeholder="Search by name, phone or Instagram" autocomplete="off">
        <button id="add-open" type="button">Add customer</button>
      </div>
      <form id="add-form" hidden style="margin-top:16px">
        <div class="fields">
          <div class="field"><label>Customer name *</label><input id="new-name" autocomplete="off"></div>
          <div class="field"><label>Phone or Instagram (optional)</label><input id="new-contact" autocomplete="off"></div>
        </div>
        <div id="add-error"></div>
        <div class="row">
          <button type="submit">Save customer</button>
          <button type="button" class="secondary" id="add-cancel">Cancel</button>
        </div>
      </form>
    </div>
    <div class="card"><ul class="list" id="results"></ul></div>`;

  const search = document.getElementById("search");
  const form = document.getElementById("add-form");

  function showResults() {
    const list = cpp("findCustomers", search.value);
    const box = document.getElementById("results");
    if (list.length === 0) {
      box.innerHTML = `<li class="muted" style="cursor:default">No customers found. You can add a new one.</li>`;
      return;
    }
    box.innerHTML = list.map((c) => `
      <li data-id="${c.id}">
        <div><strong>${esc(c.name)}</strong>${c.contact ? `<span class="muted"> · ${esc(c.contact)}</span>` : ""}</div>
        <div class="muted small">ID ${c.id} · orders: ${c.orderCount}</div>
      </li>`).join("");
  }

  search.addEventListener("input", showResults);

  document.getElementById("results").addEventListener("click", (e) => {
    const li = e.target.closest("li[data-id]");
    if (li) go("#/customer/" + li.dataset.id);
  });

  document.getElementById("add-open").addEventListener("click", () => {
    form.hidden = false;
    document.getElementById("new-name").value = search.value;
    document.getElementById("new-name").focus();
  });

  document.getElementById("add-cancel").addEventListener("click", () => {
    form.hidden = true;
  });

  form.addEventListener("submit", (e) => {
    e.preventDefault();
    const name = document.getElementById("new-name").value;
    const contact = document.getElementById("new-contact").value;
    const result = cpp("addCustomer", name, contact);
    if (result.error) {
      document.getElementById("add-error").innerHTML = `<div class="error-box">${esc(result.error)}</div>`;
      return;
    }
    saveToBrowser();
    message = result.existing
      ? `A customer with this name already exists (ID ${result.id}). Their profile is opened.`
      : "Customer added.";
    go("#/customer/" + result.id);
  });

  showResults();
  search.focus();
}

// ---------- page 2: customer profile ----------

function ordersTable(orders, withCustomer) {
  const now = today();
  let rows = "";
  for (const o of orders) {
    const late = o.stage < 5 && o.dueDate < now;
    const soon = !late && o.stage < 4 && daysLeft(o.dueDate) <= 3;
    rows += `
      <tr class="clickable ${late ? "overdue" : soon ? "soon" : ""}" data-order="${o.id}">
        <td class="due">${showDate(o.dueDate)}${late ? " · overdue" : ""}</td>
        <td>${o.id}</td>
        ${withCustomer ? `<td>${esc(o.customerName)}<div class="muted small">${esc(o.customerContact)}</div></td>` : ""}
        <td>${esc(o.title)}</td>
        <td>${badge(o.stage, o.stageName)}<div class="muted small">${o.readyCount} of ${o.itemCount} ready</div></td>
        <td class="money">${money(o.total)}</td>
      </tr>`;
  }
  return `
    <table>
      <tr>
        <th>Due date</th><th>No.</th>${withCustomer ? "<th>Customer</th>" : ""}<th>Project / occasion</th>
        <th>Status</th><th class="money">Amount</th>
      </tr>
      ${rows}
    </table>`;
}

function openOrderOnClick(box) {
  box.addEventListener("click", (e) => {
    const row = e.target.closest("tr[data-order]");
    if (row) go("#/order/" + row.dataset.order);
  });
}

function showCustomer(id) {
  const c = cpp("getCustomer", id);
  if (c.error) {
    message = c.error;
    go("#/customers");
    return;
  }

  app.innerHTML = `
    <a class="back" href="#/customers">← All customers</a>
    ${takeMessage()}
    <div class="row space">
      <div>
        <h1>${esc(c.name)}</h1>
        <p class="subtitle">${c.contact ? esc(c.contact) + " · " : ""}ID ${c.id}</p>
      </div>
      <button id="new-order" type="button">New order</button>
    </div>
    <h2>Order history</h2>
    <div class="card" id="orders">
      ${c.orders.length ? ordersTable(c.orders.slice().reverse(), false) : `<p class="muted">No orders yet.</p>`}
    </div>`;

  openOrderOnClick(document.getElementById("orders"));

  document.getElementById("new-order").addEventListener("click", () => {
    cpp("startDraft", id);
    go("#/draft");
  });
}

// ---------- page 3: new order ----------

function fieldHtml(field, value) {
  let input;
  if (field.type === "select") {
    const options = field.choices.map(([v, text]) =>
      `<option value="${v}" ${v === value ? "selected" : ""}>${esc(text)}</option>`).join("");
    input = `<select data-key="${field.key}">${options}</select>`;
  } else if (field.type === "textarea") {
    input = `<textarea data-key="${field.key}">${esc(value)}</textarea>`;
  } else {
    input = `<input data-key="${field.key}" value="${esc(value)}" ${field.type === "number" ? 'inputmode="numeric"' : ""}>`;
  }
  const wide = field.type === "textarea" ? "wide" : "";
  return `<div class="field ${wide}"><label>${esc(field.label)}</label>${input}<div class="field-error"></div></div>`;
}

function itemEditor(item) {
  let fields = "";
  for (const field of PRODUCTS[item.type].fields) {
    if (field.onlyForKind && item.options.kind !== field.onlyForKind) continue;
    fields += fieldHtml(field, item.options[field.key]);
  }
  return `
    <div class="card" data-index="${item.index}">
      <div class="item-head">
        <h3>${item.index + 1}. ${esc(item.name)}</h3>
        <button type="button" class="danger" data-remove="${item.index}">Remove item</button>
      </div>
      <div class="item">
        <div>
          <div class="fields">
            ${fields}
            <div class="field">
              <label>Quantity, pcs</label>
              <input data-quantity inputmode="numeric" value="${item.quantity}">
              <div class="field-error"></div>
            </div>
          </div>
          <p class="muted small details">${esc(item.details)}</p>
        </div>
        <div class="price">${priceTable(item.price)}</div>
      </div>
    </div>`;
}

function showDraft() {
  const draft = cpp("getDraft");
  if (draft.error) {
    go("#/customers");
    return;
  }

  const productOptions = Object.entries(PRODUCTS)
    .map(([type, p]) => `<option value="${type}">${esc(p.label)}</option>`).join("");

  app.innerHTML = `
    <a class="back" href="#/customer/${draft.customerId}">← Customer profile</a>
    <h1>New order No. ${draft.id}</h1>
    <p class="subtitle">Customer: ${esc(draft.customerName)}${draft.customerContact ? " · " + esc(draft.customerContact) : ""}</p>
    <div class="card">
      <div class="fields">
        <div class="field"><label>Project title or occasion *</label>
          <input id="title" value="${esc(draft.title)}" placeholder="For example: Wedding on 14 February"></div>
        <div class="field"><label>Due date *</label>
          <input id="due" type="date" value="${esc(draft.dueDate)}"></div>
      </div>
    </div>
    <div id="items"></div>
    <div class="card row">
      <select id="new-type" class="grow" style="width:auto">${productOptions}</select>
      <button type="button" id="add-item">Add item</button>
    </div>
    <div class="card order-total"><span>Order total</span><span id="total"></span></div>
    <div id="save-error"></div>
    <div class="row">
      <button type="button" id="save">Save order</button>
      <button type="button" class="secondary" id="cancel">Cancel</button>
    </div>`;

  const itemsBox = document.getElementById("items");

  function showItems(data) {
    if (data.items.length === 0) {
      itemsBox.innerHTML = `<div class="card muted">No items yet. Choose a product below and click “Add item”.</div>`;
    } else {
      itemsBox.innerHTML = data.items.map(itemEditor).join("");
    }
    document.getElementById("total").textContent = money(data.total);
  }

  // sends one field to C++ and shows the answer: error, new price, new total
  function sendField(input) {
    const card = input.closest("[data-index]");
    const index = Number(card.dataset.index);

    let result;
    if (input.hasAttribute("data-quantity")) {
      result = cpp("setDraftItemQuantity", index, input.value);
    } else {
      result = cpp("setDraftItemOption", index, input.dataset.key, input.value);
    }

    input.closest(".field").querySelector(".field-error").textContent = result.error;
    input.classList.toggle("invalid", !result.ok);
    card.querySelector(".price").innerHTML = priceTable(result.item.price);
    card.querySelector(".details").textContent = result.item.details;
    document.getElementById("total").textContent = money(result.total);
    return result;
  }

  // typing in text fields
  itemsBox.addEventListener("input", (e) => {
    if (e.target.tagName !== "SELECT") sendField(e.target);
  });

  itemsBox.addEventListener("change", (e) => {
    const input = e.target;
    if (input.tagName === "SELECT") {
      // a select can change the form (card type), so this item is drawn again
      const result = sendField(input);
      input.closest("[data-index]").outerHTML = itemEditor(result.item);
    } else if (input.classList.contains("invalid")) {
      // wrong value: put back the last correct value from C++
      const index = Number(input.closest("[data-index]").dataset.index);
      const item = cpp("getDraft").items[index];
      input.value = input.hasAttribute("data-quantity") ? item.quantity : item.options[input.dataset.key];
      input.classList.remove("invalid");
    }
  });

  itemsBox.addEventListener("click", (e) => {
    const button = e.target.closest("[data-remove]");
    if (button) showItems(cpp("removeDraftItem", Number(button.dataset.remove)));
  });

  document.getElementById("add-item").addEventListener("click", () => {
    showItems(cpp("addDraftItem", document.getElementById("new-type").value));
  });

  const title = document.getElementById("title");
  const due = document.getElementById("due");
  function sendInfo() {
    cpp("setDraftInfo", title.value, due.value);
  }
  title.addEventListener("input", sendInfo);
  due.addEventListener("change", sendInfo);

  document.getElementById("save").addEventListener("click", () => {
    sendInfo();
    const result = cpp("saveDraft", today());
    if (result.error) {
      document.getElementById("save-error").innerHTML = `<div class="error-box">${esc(result.error)}</div>`;
      return;
    }
    saveToBrowser();
    message = "Order saved.";
    go("#/order/" + result.orderId);
  });

  document.getElementById("cancel").addEventListener("click", () => go("#/customer/" + draft.customerId));

  showItems(draft);
}

// ---------- page 4: saved order ----------

function showOrder(id) {
  const o = cpp("getOrder", id);
  if (o.error) {
    message = o.error;
    go("#/orders");
    return;
  }

  let items = "";
  for (const item of o.items) {
    const stageOptions = stageNames.map((name, s) =>
      `<option value="${s}" ${s === item.stage ? "selected" : ""}>${esc(name)}</option>`).join("");
    items += `
      <div class="card">
        <div class="item-head">
          <h3>${item.index + 1}. ${esc(item.name)}</h3>
          <select data-stage="${item.index}" style="width:auto">${stageOptions}</select>
        </div>
        <div class="item">
          <div><p>${esc(item.details)}</p><p class="muted small">Quantity: ${item.quantity} pcs</p></div>
          <div class="price">${priceTable(item.price)}</div>
        </div>
      </div>`;
  }

  app.innerHTML = `
    <a class="back" href="#/customer/${o.customerId}">← Customer profile</a>
    ${takeMessage()}
    <h1>Order No. ${o.id} — ${esc(o.title)}</h1>
    <p class="subtitle">
      Customer: <a href="#/customer/${o.customerId}">${esc(o.customerName)}</a>${o.customerContact ? " · " + esc(o.customerContact) : ""}
      · Due: <strong>${showDate(o.dueDate)}</strong> · Created: ${showDate(o.createdDate)}
    </p>
    <div class="card row space">
      <div>Overall status: ${badge(o.stage, o.stageName)}
        <span class="muted small"> — set by the item that is furthest behind</span></div>
      <div class="muted">items ready: ${o.readyCount} of ${o.itemCount}</div>
    </div>
    <div id="order-items">${items}</div>
    <div class="card order-total"><span>Order total</span><span>${money(o.total)}</span></div>
    <button type="button" class="danger" id="delete-order">Delete order</button>`;

  document.getElementById("delete-order").addEventListener("click", () => {
    if (!confirm("Delete order No. " + o.id + "? This cannot be undone.")) return;
    const result = cpp("deleteOrder", id);
    saveToBrowser();
    message = "Order No. " + id + " deleted.";
    go("#/customer/" + result.customerId);
  });

  document.getElementById("order-items").addEventListener("change", (e) => {
    const select = e.target;
    cpp("setItemStage", id, Number(select.dataset.stage), Number(select.value));
    saveToBrowser();
    showOrder(id);
  });
}

// ---------- page 5: all orders ----------

function showOrders() {
  const stageOptions = stageNames.map((name, s) => `<option value="${s}">${esc(name)}</option>`).join("");

  app.innerHTML = `
    <h1>All orders</h1>
    <p class="subtitle">Nearest due dates first.</p>
    <div class="card row">
      <select id="filter" style="width:auto">
        <option value="active">In progress (all except “Delivered”)</option>
        <option value="all">All orders</option>
        ${stageOptions}
      </select>
      <input id="order-search" class="grow" style="width:auto" placeholder="Customer name" autocomplete="off">
    </div>
    <div class="card" id="orders-box"></div>`;

  const filter = document.getElementById("filter");
  const search = document.getElementById("order-search");
  filter.value = ordersFilter.stage;
  search.value = ordersFilter.text;

  function showList() {
    ordersFilter = { stage: filter.value, text: search.value };
    const orders = cpp("listOrders", filter.value, search.value);
    document.getElementById("orders-box").innerHTML =
      orders.length ? ordersTable(orders, true) : `<p class="muted">No orders found.</p>`;
  }

  filter.addEventListener("change", showList);
  search.addEventListener("input", showList);
  openOrderOnClick(document.getElementById("orders-box"));
  showList();
}

// ---------- footer ----------

function setupFooter() {
  // on GitHub Pages the address is USER.github.io/REPO -> link to github.com/USER/REPO
  if (location.hostname.endsWith(".github.io")) {
    const user = location.hostname.split(".")[0];
    const repo = location.pathname.split("/")[1];
    const link = document.getElementById("source-link");
    link.href = `https://github.com/${user}/${repo}`;
    link.hidden = false;
  }

  document.getElementById("reset-demo").addEventListener("click", async () => {
    if (!confirm("Delete all changes in this browser and load the demo data again?")) return;
    await loadDemo();
    message = "Demo data restored.";
    go("#/customers");
  });
}

start();
