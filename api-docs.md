# NSAA PLAY API Documentation

## Base URL

```
https://nsaa-api.rhyoliteprime.com/api/v1
```

---

# 🔗 Endpoints

## 1. Register Affiliate Partner

**POST** `/affiliate-partners/register`

### Description

Registers a new affiliate partner in the system.

### Headers

```
Content-Type: application/json
```

### Request Body

```json
{
  "businessName": "Gulf Systems Integrators",
  "contactName": "Omar Hassan",
  "contactEmail": "omar@gulfsys.ae",
  "contactPhone": "+971 4 567 8901",
  "partnershipType": "distributor",
  "country": "UAE",
  "regionOrState": "Dubai"
}
```

### Response

* `200 OK` – Partner registered successfully
* `400 Bad Request` – Validation error

---

## 2. Submit Contact Message

**POST** `/cms/contact`

### Description

Allows users to send inquiries or complaints via the CMS contact form.

### Headers

```
Content-Type: application/json
```

### Request Body

```json
{
  "name": "Kojo Atta Frimpong",
  "email": "kojo.frimpong@example.com",
  "phoneNumber": "+233 27 345 6789",
  "message": "I was charged twice for my subscription this month. Transaction reference: TXN-20231201-001."
}
```

### Response

* `200 OK` – Message received
* `400 Bad Request` – Invalid input

---

## 3. Create Support Ticket

**POST** `/support/create-ticket`

### Description

Creates a support ticket for technical or general issues.


### Request Body

```json
{
  "name": "Emily Zhang",
  "email": "emily.zhang@fenghuang.cn",
  "phoneNumber": "+86 138 1234 5678",
  "subject": "Dashboard",
  "category": "technical",
  "message": "Dashboard widgets are not loading. WebSocket connection failed."
}
```

### Fields

* `subject`: Topic of the issue
* `category`: e.g. `technical`, `billing`, `general`

### Response

* `200 OK` – Ticket created
* `400 Bad Request` – Validation error

---

## 4. Get Hero Slides

**GET** `/cms/hero-slides`

### Description

Fetches hero section slides for the frontend.


### Response

```json
[
  {
    "id": "string",
    "title": "string",
    "imageUrl": "string",
    "description": "string"
  }
]
```

---

## 5. Get Banners

**GET** `/cms/banners`

### Description

Retrieves promotional banners.


### Response

```json
[
  {
    "id": "string",
    "imageUrl": "string",
    "link": "string"
  }
]
```

---

## 6. Get FAQs

**GET** `/cms/faqs?pageNo=1&pageSize=5`

### Description

Fetches paginated FAQ entries.

### Query Parameters

| Parameter | Type | Description    |
| --------- | ---- | -------------- |
| pageNo    | int  | Page number    |
| pageSize  | int  | Items per page |

### Headers


### Response

```json
{
  "pageNo": 1,
  "pageSize": 5,
  "total": 50,
  "data": [
    {
      "question": "string",
      "answer": "string"
    }
  ]
}
```

---

## 7. Get Weekly Raffle Schedule

**GET** `/raffle/get-weekly-schedule`

### Description

Returns all raffles scheduled within the current week (Sunday 00:00:00 – Saturday 23:59:59 local time). Each entry includes enriched raffle details alongside a human-readable `name` label that reflects the raffle's current status.

### Example Request

```
GET /raffle/get-weekly-schedule
```

### Headers

_None required._

### Response

```json
{
  "success": true,
  "result": [
    {
      "raffleId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
      "name": "Midweek Mega Draw (Wednesday)",
      "slug": "midweek-mega-draw-20260609",
      "status": "SCHEDULED",
      "dayOfWeek": "Wednesday",
      "prizePool": "5000.00",
      "totalEntries": 120,
      "winningColour": "Gold",
      "startsAt": "2026-06-11 10:00:00",
      "endsAt": "2026-06-11 22:00:00",
      "drawTime": "2026-06-11 22:30:00"
    },
    {
      "raffleId": "b2c3d4e5-f6a7-8901-bcde-f12345678901",
      "name": "Weekend Special (Ongoing)",
      "slug": "weekend-special-20260613",
      "status": "ACTIVE",
      "dayOfWeek": "Saturday",
      "prizePool": "12000.00",
      "totalEntries": 834,
      "winningColour": "",
      "startsAt": "2026-06-13 08:00:00",
      "endsAt": "2026-06-13 23:00:00",
      "drawTime": "2026-06-13 23:30:00"
    }
  ]
}
```

### `name` Label Convention

| Raffle Status        | Appended label    |
| -------------------- | ----------------- |
| `SCHEDULED`          | `(Day of Week)`   |
| `ACTIVE`             | `(Ongoing)`       |
| `CLOSED` / `COMPLETED` | `(Done)`       |

### Response Fields

| Field           | Type    | Description                                              |
| --------------- | ------- | -------------------------------------------------------- |
| `raffleId`      | string  | Unique raffle identifier (UUID)                          |
| `name`          | string  | Display name with status/day label appended              |
| `slug`          | string  | URL-friendly identifier                                  |
| `status`        | string  | One of `SCHEDULED`, `ACTIVE`, `CLOSED`, `COMPLETED`     |
| `dayOfWeek`     | string  | Plain weekday name of the raffle's start time            |
| `prizePool`     | string  | Current prize pool value (decimal string)                |
| `totalEntries`  | integer | Total number of entries recorded                         |
| `winningColour` | string  | Winning colour name; empty until draw is executed        |
| `startsAt`      | string  | Raffle open time (`YYYY-MM-DD HH:MM:SS`)                 |
| `endsAt`        | string  | Raffle close time (`YYYY-MM-DD HH:MM:SS`)                |
| `drawTime`      | string  | Scheduled draw execution time (`YYYY-MM-DD HH:MM:SS`)   |

### Response Codes

* `200 OK` – Weekly schedule returned (empty array if no raffles exist this week)
* `500 Internal Server Error` – Database or server error

---


## 8. Check Customer Account Status

**GET** `/auth/customer/check-account-status`

### Description

Checks whether a customer account exists for the given phone number, and whether a PIN has been set on that account. Useful for determining the correct auth flow (new registration vs. existing login vs. PIN setup).

### Query Parameters

| Parameter   | Type   | Required | Description              |
| ----------- | ------ | -------- | ------------------------ |
| phoneNumber | string | ✅ Yes   | Customer's phone number  |

### Example Request

```
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/auth/customer/check-account-status?phoneNumber=0244256444'
```

### Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "hasAccount": false,
    "hasPin": false,
    "requestId": "92b54e36-3e5a-8839-ec3d-ef94db60e927"
  },
  "success": true,
  "targetUrl": ""
}
```

### Response Fields

| Field        | Type    | Description                                                    |
| ------------ |---------|----------------------------------------------------------------|
| `hasAccount` | boolean | `true` if a customer record exists for the given phone number  |
| `hasPin`     | boolean | `true` if the customer has set a secret PIN; `false` otherwise |
| `requestId`  | string  | A unique identifier for this authentication attempt.           |
| `success`    | boolean | `true` on a successful lookup (even if no account is found)    |

### Response Codes

* `200 OK` – Lookup completed; check `hasAccount` and `hasPin` in `result`
* `500 Internal Server Error` – Database or server error

---

## 9. Send OTP

**POST** `/auth/customer/send-otp`

### Description

Generates a one-time password (OTP) and sends it to the customer's phone number via SMS. Requires a valid `requestId` obtained from the **Check Customer Account Status** endpoint. The `requestId` is short-lived (180 seconds) and single-use.

The User will dial *2727# -> Account -> Get Login PIN , to retrieve the Login PIN

### Request Body

| Field       | Type   | Required | Description                                                                     |
| ----------- | ------ | -------- | ------------------------------------------------------------------------------- |
| phoneNumber | string | ✅ Yes   | Customer's phone number                                                         |
| requestId   | string | ✅ Yes   | The `requestId` returned by `/auth/customer/check-account-status`               |

### Example Request

```bash
curl --location 'http://localhost:5044/api/v1/auth/customer/send-otp' \
--header 'Content-Type: application/json' \
--data '{
    "phoneNumber": "0553859517",
    "requestId": "92b54e36-3e5a-8839-ec3d-ef94db60e927"
}'
```

### Response

```json
{
    "error": null,
    "message": "OTP sent successfully.",
    "result": {
        "phoneNumber": "0553859517",
        "requestId": "425d4930-d902-0321-3aeb-8de8b4d4abc2"
    },
    "success": true,
    "targetUrl": ""
}
```

### Response Fields

| Field       | Type    | Description                                                                       |
| ----------- | ------- | --------------------------------------------------------------------------------- |
| `requestId` | string  | The same `requestId` to pass in subsequent steps (e.g. PIN verification)          |
| `phoneNumber` | string | The phone number the OTP was dispatched to                                      |
| `success`   | boolean | `true` when the OTP was generated and the SMS dispatched successfully             |
| `message`   | string  | Human-readable status message                                                     |

### Response Codes

* `200 OK` – OTP generated and SMS sent
* `400 Bad Request` – Missing or invalid `requestId` / `phoneNumber`
* `401 Unauthorized` – `requestId` has expired or was already consumed
* `500 Internal Server Error` – SMS delivery failure or server error

---

## 10. Customer Token (PIN Login)

**POST** `/auth/customer/token`

### Description

Authenticates a customer using their phone number and PIN. Returns a signed JWT token on success. Use this endpoint for returning customers who have already set a PIN.


### Request Body

| Field       | Type   | Required | Description                                                   |
| ----------- | ------ | -------- | ------------------------------------------------------------- |
| phoneNumber | string | ✅ Yes   | Customer's phone number                                       |
| requestId   | string | ✅ Yes   | The `requestId` from `/auth/customer/check-account-status`    |
| pin         | string | ✅ Yes   | The customer's 6-digit PIN                                    |

### Example Request

```bash
curl --location 'http://localhost:5044/api/v1/auth/customer/token' \
--header 'Content-Type: application/json' \
--data '{
    "phoneNumber": "0553859517",
    "requestId": "3d194304-4ef8-8650-964b-02ccf927d0df",
    "pin": "310964"
}'
```

### Response

```json
{
    "error": null,
    "message": "Authentication successful",
    "result": {
        "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiOGU5Yzg4MTUtNmFmZC00ZGM5LTgzYzItOGQyNTNjMDk3MTQyIiwiZXhwIjoxNzkxNjM4OTgwLCJpYXQiOjE3ODEyNzA5ODAsImlzcyI6Ik5TQUFfUExBWSJ9.AAZincU7M2IUtijaHySo7Naiuf7cT75HG2fkVU0iaos",
        "requestId": "88343",
        "phoneNumber": "0503344544"
    },
    "success": true,
    "targetUrl": ""
}
```

## 11. Set Customer PIN

**POST** `/auth/customer/set-pin`

### Description

Sets a PIN for a customer account. Used during initial registration or PIN reset flows, after OTP verification has been completed.


### Request Body

| Field       | Type   | Required | Description                                                   |
| ----------- | ------ | -------- | ------------------------------------------------------------- |
| phoneNumber | string | ✅ Yes   | Customer's phone number                                       |
| requestId   | string | ✅ Yes   | The `requestId` from the preceding OTP verification step      |
| pin         | string | ✅ Yes   | The 6-digit PIN to set for the account                        |

### Example Request

```bash
curl --location 'http://localhost:5044/api/v1/auth/customer/set-pin' \
--header 'Content-Type: application/json' \
--data '{
    "phoneNumber": "0553859517",
    "requestId": "3d194304-4ef8-8650-964b-02ccf927d0df",
    "pin": "310964"
}'
```

### Example Request

```json
{
    "error": null,
    "message": "PIN set successfully.",
    "result": null,
    "success": true,
    "targetUrl": ""
}
```


## 13. Reset Customer PIN

**POST** `/auth/customer/reset-pin`

### Description

Sets a PIN for a customer account. Used during initial registration or PIN reset flows, after OTP verification has been completed.


### Request Body

| Field       | Type   | Required | Description                                                   |
| ----------- | ------ | -------- | ------------------------------------------------------------- |
| phoneNumber | string | ✅ Yes   | Customer's phone number                                       |
| requestId   | string | ✅ Yes   | The `requestId` from the preceding OTP verification step      |
| pin         | string | ✅ Yes   | The 6-digit PIN to set for the account                        |

### Example Request

```bash
curl --location 'http://localhost:5044/api/v1/auth/customer/set-pin' \
--header 'Content-Type: application/json' \
--data '{
    "phoneNumber": "0553859517",
    "requestId": "3d194304-4ef8-8650-964b-02ccf927d0df",
    "oldPin": "310964",
    "newPin": "310964",
    
}'
```

### Example Response

```json
{
    "error": null,
    "message": "Reset set successfully.",
    "result": null,
    "success": true,
    "targetUrl": ""
}
```


## 14. List Colors

**POST** `/cms/colours/list-all`

### Description

Gets a list of all available colours.

### Example Request

```bash
curl --location 'http://localhost:5044/api/v1/cms/colours/list-all'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "code": "RED",
        "emoji": "🔴",
        "id": "74dabff4-079e-5d0b-8ae9-1b2a536dbf11",
        "name": "Red",
        "sortOrder": 1
      },
      {
        "code": "GOLD",
        "emoji": "🟡",
        "id": "0bcb27f1-4593-53b4-bbcd-2ed6cf8bde73",
        "name": "Gold",
        "sortOrder": 2
      },
      {
        "code": "GREEN",
        "emoji": "🟢",
        "id": "937a22c0-f7d5-57bd-9a98-71e44fc7b78a",
        "name": "Green",
        "sortOrder": 3
      },
      {
        "code": "BLACK_STAR",
        "emoji": "⭐",
        "id": "9de57805-008a-5ec4-ae79-ec94b4818173",
        "name": "Black Star",
        "sortOrder": 4
      }
    ],
    "lowerBound": 1,
    "pageNo": 1,
    "pageSize": 10,
    "totalCount": 4,
    "totalPages": 1,
    "upperBound": 4
  },
  "success": true,
  "targetUrl": ""
}
```





# ⚠️ Error Handling


Common error responses:

```json
{
  "statusCode": 400,
  "message": "Validation error",
  "errors": []
}
```

```json
{
  "statusCode": 401,
  "message": "Unauthorized"
}
```
