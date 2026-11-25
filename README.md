# Print Design Studio — Customers and Orders

IFS4277 Programming Languages project.
A web application with a C++ core: all prices, discounts and checks are written in C++
and run in the browser as WebAssembly (compiled with Emscripten).

## Folders

| Folder / file | What is inside |
|---|---|
| `src/` | C++ code |
| `src/OrderItem.*` | base class for all products: quantity, discount, stages |
| `src/MagazineItem.*`, `NewspaperItem.*`, `PosterItem.*`, `InvitationItem.*`, `CardItem.*`, `EnvelopeItem.*` | products, each has its own `calculatePrice()` |
| `src/Order.*` | one order with many items, `calculateTotal()` |
| `src/Customer.*` | customer with orders (add, find, delete an order) |
| `src/Studio.*` | all customers and orders, the page talks only to this class |
| `src/Tariffs.*` | reads prices from `web/data/tariffs.json` |
| `src/Helpers.*` | small text functions |
| `src/bindings.cpp` | connects `Studio` to JavaScript |
| `src/lib/json.hpp` | JSON library (nlohmann/json) |
| `web/` | the site: `index.html`, `style.css`, `app.js` |
| `web/data/tariffs.json` | all prices |
| `web/data/demo.json` | demo customers |
| `tests/test_prices.cpp` | checks of the price rules |
| `build.sh` | build script |
| `.github/workflows/deploy.yml` | GitHub builds and publishes the site after every push |

## Exam topics

- Encapsulation — `OrderItem::setQuantity()` in `src/OrderItem.cpp`
- Inheritance — `class CardItem : public OrderItem` in `src/CardItem.h`
- Polymorphism — `Order::calculateTotal()` in `src/Order.cpp`

## Commands

- `bash build.sh test` — check the prices
- `bash build.sh` — build C++ to WebAssembly (needs Emscripten)
- `python3 -m http.server 8000 -d web` — open the site at http://localhost:8000
