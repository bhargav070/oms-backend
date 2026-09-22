# Tradex Backend

The `backend` directory contains Tradex's C++17 order-management service. It exposes a small JSON HTTP API, keeps exchange session state, normalizes market and account responses, and forwards trading commands to exchange adapters.

The current adapter is Deribit. The service is designed so additional crypto venues and broker integrations can be added behind the exchange and market-data interfaces without changing the frontend contract.

## What it does today

- Starts a Boost.Beast HTTP server (default port `8080`).
- Loads configuration from `backend/.env` or the process environment.
- Connects to Deribit using the configured testnet or mainnet host.
- Serves ticker and candle data for the dashboard.
- Supports exchange login/session creation, account summaries, positions, open orders, order history, order placement, and order cancellation.
- Adds permissive CORS headers for local frontend development.
- Keeps order history in process memory for the current run.

Persistence, streaming events, risk limits, and additional venue adapters are planned. Do not treat the current service as production-ready execution infrastructure without adding those controls.

## Architecture

```text
HTTP request
    |
    v
HttpServer (Boost.Beast, CORS, bearer header)
    |
    v
Router (method/path/query dispatch)
    |
    v
OMSService (sessions, order commands, response mapping)
    |\
    | +--> SessionManager (in-memory auth sessions)
    |
    +----> ExchangeFactory --> IExchange / IMarketDataProvider
                                      |
                                      +--> DeribitClient
```

Important source areas:

| Area | Location | Responsibility |
| --- | --- | --- |
| Application entry point | `src/main.cpp` | Loads settings and starts the server |
| HTTP transport | `src/api/` | Accepts requests, handles CORS, dispatches routes |
| Application service | `src/service/` | Coordinates sessions, market data, and orders |
| Domain contracts | `src/domain/` and `src/interfaces/` | Orders, market data, and provider interfaces |
| Authentication | `src/auth/` | In-memory bearer-token sessions |
| Adapter selection | `src/factory/` | Resolves the configured exchange |
| Exchange integration | `src/adapters/deribit/` | Deribit REST/RPC calls and response mapping |
| Configuration | `config/` | Environment loading and typed settings |

See [`docs/backend-architecture.md`](../docs/backend-architecture.md) for the target module boundaries and persistence/event direction.

## API

All responses are JSON. Public market routes use the configured/default Deribit connection. Private routes require `Authorization: Bearer <token>`.

| Method | Endpoint | Auth | Purpose |
| --- | --- | --- | --- |
| `GET` | `/api/v1/health` | No | Returns `{"status":"ok"}` |
| `GET` | `/api/v1/market/ticker?instrument=BTC-PERPETUAL` | No | Current bid, ask, last price, and quantities |
| `GET` | `/api/v1/market/candles?instrument=BTC-PERPETUAL&resolution=15&from=<ms>&to=<ms>` | No | OHLCV candles; defaults to the previous 24 hours |
| `POST` | `/api/v1/auth/login` | No | Creates a session from exchange credentials |
| `POST` | `/api/v1/auth/exchange` | Yes | Connects the configured account |
| `GET` | `/api/v1/account/summary?currency=BTC` | Yes | Equity, funds, margin, and PnL snapshot |
| `GET` | `/api/v1/positions` | Yes | Current exchange positions |
| `GET` | `/api/v1/orders/open` | Yes | Open orders |
| `GET` | `/api/v1/orders/history` | Yes | In-memory/order-provider history |
| `POST` | `/api/v1/order/place` | Yes | Places an order through the active provider |
| `POST` | `/api/v1/order/cancel` | Yes | Cancels an order by provider order id |

Example login payload:

```json
{
  "exchange": "deribit",
  "api_key": "your-key",
  "api_secret": "your-secret",
  "is_testnet": true
}
```

Example order cancellation payload:

```json
{
  "order_id": "provider-order-id"
}
```

The exact order placement fields follow the domain order model in [`src/domain/Order.hpp`](src/domain/Order.hpp). The frontend is the reference client for the currently supported order payload.

## Configuration

Copy the example file and keep real credentials out of Git:

```powershell
Copy-Item backend\.env.example backend\.env
```

Available variables:

| Variable | Default/example | Description |
| --- | --- | --- |
| `APP_PORT` | `8080` | HTTP listen port |
| `DEFAULT_EXCHANGE` | `deribit` | Exchange selected by the configured account flow |
| `DERIBIT_API_KEY` | placeholder | Deribit API key |
| `DERIBIT_API_SECRET` | placeholder | Deribit API secret |
| `DERIBIT_BASE_URL` | `test.deribit.com` | Deribit host |
| `DERIBIT_USE_TESTNET` | `true` | Selects testnet behavior |

Use Deribit testnet credentials first. Mainnet access should be isolated and must have risk checks, reconciliation, audit logging, and operational controls before being considered safe for live trading.

## Build and run on Windows

Prerequisites:

- Visual Studio C++ build tools
- CMake 3.20 or later
- The repository-local `vcpkg` checkout and `x64-windows` triplet

From the repository root:

```powershell
cmake -S backend -B backend/build-tradex -DCMAKE_BUILD_TYPE=Release
cmake --build backend/build-tradex --config Release --parallel 2
backend\build-tradex\bin\Release\oms.exe
```

The executable reads `backend/.env` when started from the repository root. Confirm it is reachable before starting the frontend:

```powershell
Invoke-RestMethod http://localhost:8080/api/v1/health
```

## Development notes

- The server currently handles requests with detached per-connection threads.
- Private route logging records request paths and status, but credentials must never be logged.
- REST polling is the current transport. A normalized event stream is planned for quotes, orders, fills, positions, and strategy signals.
- The next backend milestones are separated market-data/execution ports, durable persistence, risk controls, reconciliation, and provider capability discovery.

## Related documentation

- [`docs/backend-architecture.md`](../docs/backend-architecture.md)
- [`docs/workflows.md`](../docs/workflows.md)
- [`docs/implementation-plan.md`](../docs/implementation-plan.md)
- [`frontend/README.md`](../frontend/README.md)