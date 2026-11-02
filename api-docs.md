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

| Field       | Type   | Required | Description                                                |
| ----------- | ------ | -------- |------------------------------------------------------------|
| phoneNumber | string | ✅ Yes   | Customer's phone number                                    |
| requestId   | string | ✅ Yes   | The `requestId` from `/auth/customer/check-account-status` |
| pin         | string | ✅ Yes   | The customer's 4-6 digit PIN                               |

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


## 11. Reset Customer PIN

**POST** `/auth/customer/reset-pin`

### Description

Sets a PIN for a customer account. Used during initial registration or PIN reset flows, after OTP verification has been completed.


### Request Body

| Field       | Type   | Required | Description                                              |
|-------------| ------ | -------- |----------------------------------------------------------|
| phoneNumber | string | ✅ Yes   | Customer's phone number                                  |
| requestId   | string | ✅ Yes   | The `requestId` from the preceding OTP verification step |
| oldPin      | string | ✅ Yes   | The 4-6 digit Old PIN to set for the account             |
| newPin      | string | ✅ Yes   | The 4-6 digit New PIN to set for the account             |

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

## 13. Get Customer Transactions History

**GET** `/customers/get-transaction-history`

### Description

Gets a paginated list of transactions for a customer.

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/customers/get-transaction-history' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiMzExYzg4MDQtMmE1My00Yjg5LWE3OWUtMDE4NDc0N2FjZTA1IiwiZXhwIjoxNzkzMTk5NTcxLCJpYXQiOjE3ODI4MzE1NzEsImlzcyI6Ik5TQUFfUExBWSJ9.onHw4U5Wdktwe2ZQLmdN8fjpZ1WRiDYXaOhe3eVLhOs'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "id": "311c8804-2a53-4b89-a79e-0184747ace05",
        "ticketNumber": "CM-2024-001",
        "drawId": "draw_123",
        "raffleId": "raffle_456",
        "colourId": "311c8804-2a53-4b89-a79e-0184747ace02",
        "colourName": "Red",
        "entryFee": 10.50,
        "quantity": 2,
        "totalAmount": 21.00,
        "paymentStatus": "Paid",
        "createdAt": "2024-01-15 14:30:00",
        "ticketAlias": "G87"
      },
      {
        "id": "311c8804-2a53-4b89-a79e-0184747ace09",
        "ticketNumber": "TK-2024-002",
        "drawId": "draw_123",
        "raffleId": "raffle_456",
        "colourId": "col_blue",
        "colourName": "Gold",
        "entryFee": 10.50,
        "quantity": 1,
        "totalAmount": 10.50,
        "paymentStatus": "Pending",
        "createdAt": "2024-01-15 15:45:00",
        "ticketAlias": "G98"
      }
    ]
  },
  "success": true,
  "targetUrl": ""
}
```


## 14. Get Customer Stats

**GET** `/customers/get-stats`

### Description

Gets basic customer statistics.

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/customers/get-stats' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiMzExYzg4MDQtMmE1My00Yjg5LWE3OWUtMDE4NDc0N2FjZTA1IiwiZXhwIjoxNzkzMTk5NTcxLCJpYXQiOjE3ODI4MzE1NzEsImlzcyI6Ik5TQUFfUExBWSJ9.onHw4U5Wdktwe2ZQLmdN8fjpZ1WRiDYXaOhe3eVLhOs'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "numberOfEntries": 0,
    "numberOfWins": 0,
    "totalSpent": "0.00",
    "totalWon": "0.00"
  },
  "success": true,
  "targetUrl": ""
}
```


## 15. Get Customer Profile Details

**GET** `/customers/profile-details`

### Description

Gets customer profile details.

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/customers/profile-details' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiMzExYzg4MDQtMmE1My00Yjg5LWE3OWUtMDE4NDc0N2FjZTA1IiwiZXhwIjoxNzkzMTk5NTcxLCJpYXQiOjE3ODI4MzE1NzEsImlzcyI6Ik5TQUFfUExBWSJ9.onHw4U5Wdktwe2ZQLmdN8fjpZ1WRiDYXaOhe3eVLhOs'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "accountName": "",
    "accountNumber": "",
    "accountProvider": "",
    "accountType": "",
    "createdAt": "2026-06-30 14:59:15.261228",
    "email": "",
    "firstName": "",
    "ghanaCardNumber": "",
    "id": "311c8804-2a53-4b89-a79e-0184747ace05",
    "isActive": true,
    "lastName": "",
    "msisdn": "233503789505"
  },
  "success": true,
  "targetUrl": ""
}
```

## 16. Buy Ticket (Authenticated Customer)

**POST** `/customers/buy-ticket`

### Description

Buys a ticket for a customer. Returns a checkout URL and ticket details.

### Example Request

```bash
curl --location 'http://localhost:5044/api/v1/customers/buy-ticket' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiNTM0YzYxMjctYTcxZS00ZmEwLWExODctM2ZlYzFkZTIwMDJhIiwiZXhwIjoxNzkzNzgzODk3LCJpYXQiOjE3ODM0MTU4OTcsImlzcyI6Ik5TQUFfUExBWSJ9.fZUlOVquCmzh1CPHZgnMo_wmESknTkYpBRzZdYJWxAs' \
--header 'Content-Type: application/json' \
--header 'Cookie: auth_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6IndlaS56aGFuZ0BleGFtcGxlLmNvbSIsImV4cCI6MTc4MTY1MTQwOSwiaWF0IjoxNzc5MDU5NDA5LCJpc3MiOiJOb3MiLCJ1c2VySWQiOiJjMDY2YTZlYy0xZmQxLTQ0NzctOTY0MC02NjQyMjM4Y2JlMzAiLCJ1c2VybmFtZSI6IndlaXpoYW5nIn0.vwUVS8kDZmL-b7fdysv-URy-r5voL8tv9i3YAiA_gIU' \
--data '{
    "raffleId": "8990662a-e40f-4344-9b56-1fd31680aa64",
    "selectedOption": "gold",
    "ticketGroupId" : "0bcb27f1-4593-53b4-bbcd-2ed6cf8bde74"
}'
```

### Example Response

```json
 {
  "error": null,
  "message": "Ticket created successfully. Please complete payment.",
  "result": {
    "checkoutUrl": "https://pay.hubtel.com/f3fda1ca4a27424786034b7174d83467",
    "paymentReference": "690864ea-a3c9-0d95-85c9-56fddd5197a3",
    "selectedColour": "Gold",
    "ticketId": "7f6361c7-76dd-4c2c-b86f-8c0e52352893",
    "ticketNumber": "20260707083625",
    "ticketAlias": "WDW-004"
  },
  "success": true,
  "targetUrl": ""
}
```


## 17. Get Customer Raffle History

### GET `/customers/get-raffle-history`

### Description

Gets a paginated list of raffle entries a customer has participated in.

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/customers/get-raffle-history' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiMzExYzg4MDQtMmE1My00Yjg5LWE3OWUtMDE4NDc0N2FjZTA1IiwiZXhwIjoxNzkzMTk5NTcxLCJpYXQiOjE3ODI4MzE1NzEsImlzcyI6Ik5TQUFfUExBWSJ9.onHw4U5Wdktwe2ZQLmdN8fjpZ1WRiDYXaOhe3eVLhOs'
```

### Example Response

```json
 {
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "drawTime": "2026-06-03 17:10:00",
        "raffleId": "68afb58b-1e8c-42de-9065-4e8c92fe9629",
        "raffleName": "Wealth Drop Wednesday",
        "status": "ACTIVE",
        "ticketsBought": 4,
        "totalSpent": "2.40"
      }
    ],
    "lowerBound": 1,
    "pageNo": 1,
    "pageSize": 10,
    "totalCount": 1,
    "totalPages": 1,
    "upperBound": 1
  },
  "success": true,
  "targetUrl": ""
}
```

## 18. Buy Ticket (Guest Customer)

**POST** `/customers/buy-guest-ticket`

### Description

Buys a ticket for a guest customer. Returns a checkout URL and ticket details. Automatically creates an account for the guest and return a JWT for login.

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/customers/buy-guest-ticket' \
--header 'Content-Type: application/json' \
--data '{
    "raffleId": "8990662a-e40f-4344-9b56-1fd31680aa64",
    "selectedOption": "gold",
    "phoneNumber": "0245444123",
    "ticketGroupId" : "0bcb27f1-4593-53b4-bbcd-2ed6cf8bde74"
}'
```

### Example Response

```json
 {
  "error": null,
  "message": "Ticket created successfully. Please complete payment.",
  "result": {
    "checkoutUrl": "https://pay.hubtel.com/f3fda1ca4a27424786034b7174d83467",
    "paymentReference": "690864ea-a3c9-0d95-85c9-56fddd5197a3",
    "selectedColour": "Gold",
    "ticketId": "7f6361c7-76dd-4c2c-b86f-8c0e52352893",
    "ticketNumber": "20260707083625",
    "ticketAlias": "WDW-004",
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjdXN0b21lcklkIjoiZDQyYzk5YTgtZDdkYi00NmI1LTlhZGMtMmIxYzUwMmFkZTcxIiwiZXhwIjoxNzkzODk2NzIwLCJpYXQiOjE3ODM1Mjg3MjAsImlzcyI6Ik5TQUFfUExBWSJ9.Ka6LdjUIQQHGiC54Es5IaVTiZBnpOAkYgJ5WG66yCIY"
  },
  "success": true,
  "targetUrl": ""
}
```

## 20. Get Ticket Groups

**POST** `/cms/tickets/get-groups`

### Description

Gets a list of all Ticket Groups & Prices  

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/cms/tickets/get-groups'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "id": "e8036a4a-1879-4fdf-88d3-b65a53c42ed6",
        "isActive": true,
        "name": "10 Tickets",
        "quantity": 10,
        "unitPrice": "5.00"
      },
      {
        "id": "961ae756-86e2-43c9-8667-c6c194f75526",
        "isActive": true,
        "name": "5 Tickets",
        "quantity": 5,
        "unitPrice": "5.00"
      },
      {
        "id": "18439554-2a20-4c34-8f4b-95214b6cef41",
        "isActive": true,
        "name": "2 Tickets",
        "quantity": 2,
        "unitPrice": "5.00"
      },
      {
        "id": "e51ff197-6831-4291-b490-d7cd0a9b027f",
        "isActive": true,
        "name": "1 Ticket",
        "quantity": 1,
        "unitPrice": "5.00"
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



## 20. Get recent draws

**POST** `/cms/get-recent-draws`

### Description

Gets a list of all Recent draws

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/cms/get-recent-draws'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "alias": "tuesday-bliss",
        "drawCode": "20260428-1830",
        "drawDate": "2026-04-28 18:30:00",
        "drawTime": null,
        "id": "701a21f7-a711-482d-86ae-2d77a5697070",
        "status": "COMPLETED",
        "totalEntries": 35,
        "totalPool": "7.00",
        "totalPrices": "11000.00",
        "winningColour": "Black Star",
        "winningColourId": "9de57805-008a-5ec4-ae79-ec94b4818173"
      },
      {
        "alias": "mabango-grand-2026",
        "drawCode": "20250118-2005",
        "drawDate": "2025-01-18 20:05:00",
        "drawTime": null,
        "id": "78653b5c-212c-4295-a55d-6b88b4be9e98",
        "status": "COMPLETED",
        "totalEntries": 0,
        "totalPool": "0.00",
        "totalPrices": "14000.00",
        "winningColour": null,
        "winningColourId": "937a22c0-f7d5-57bd-9a98-71e44fc7b78a"
      },
      {
        "alias": "january-grand-2026",
        "drawCode": "20260118-2005",
        "drawDate": "2026-01-18 20:05:00",
        "drawTime": null,
        "id": "876e172c-2b44-440e-887b-0f51634dddf9",
        "status": "COMPLETED",
        "totalEntries": 105,
        "totalPool": "0.00",
        "totalPrices": "15001.56",
        "winningColour": "Red",
        "winningColourId": "74dabff4-079e-5d0b-8ae9-1b2a536dbf11"
      },
      {
        "alias": "sunday-jackpot-2026",
        "drawCode": "20260111-2105",
        "drawDate": "2026-01-11 21:05:00",
        "drawTime": null,
        "id": "35c1c3f4-c3a5-4619-9007-1c32765c9940",
        "status": "COMPLETED",
        "totalEntries": 0,
        "totalPool": "0.00",
        "totalPrices": "15000.06",
        "winningColour": "Red",
        "winningColourId": "74dabff4-079e-5d0b-8ae9-1b2a536dbf11"
      },
      {
        "alias": "friday-night-draw-2026",
        "drawCode": "19691231-2359",
        "drawDate": "1969-12-31 23:59:59",
        "drawTime": null,
        "id": "4dd647a4-75ca-487d-8e2c-eab0428d1021",
        "status": "COMPLETED",
        "totalEntries": 0,
        "totalPool": "0.00",
        "totalPrices": "14000.00",
        "winningColour": "Gold",
        "winningColourId": "0bcb27f1-4593-53b4-bbcd-2ed6cf8bde73"
      }
    ],
    "lowerBound": 1,
    "pageNo": 1,
    "pageSize": 10,
    "totalCount": 5,
    "totalPages": 1,
    "upperBound": 5
  },
  "success": true,
  "targetUrl": ""
}
```


## 21. Get recent winners

**POST** `/cms/get-recent-winners`

### Description

Gets a list of all Recent Winners

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/cms/get-recent-winners'
```

### Example Response

```json
 {
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "actualPayout": "150.00",
        "customerId": "c3d4e5f6-a7b8-9012-cdef-345678901234",
        "drawId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
        "id": "cb311f8f-d8ac-42f6-a511-cce98af8cbac",
        "phoneNumber": "+23*******890",
        "prizeAmount": "150.00",
        "ticketAlias": "T-001",
        "ticketId": "b2c3d4e5-f6a7-8901-bcde-f23456789012"
      },
      {
        "actualPayout": "300.00",
        "customerId": "b8c9d0e1-f2a3-4567-a1b3-890123456789",
        "drawId": "c3d4e5f6-a7b8-9012-cdef-345678901234",
        "id": "ddfafc90-6ddf-44ed-8e10-b5231b077c77",
        "phoneNumber": "+23*******111",
        "prizeAmount": "300.00",
        "ticketAlias": "T-006",
        "ticketId": "a7b8c9d0-e1f2-3456-a0b2-789012345678"
      },
      {
        "actualPayout": "500.00",
        "customerId": "e5f6a7b8-c9d0-1234-ef00-567890123456",
        "drawId": "b2c3d4e5-f6a7-8901-bcde-f23456789012",
        "id": "eefdbcde-41c7-431e-a18f-b565c1c2756f",
        "phoneNumber": "+23*******455",
        "prizeAmount": "500.00",
        "ticketAlias": "T-003",
        "ticketId": "d4e5f6a7-b8c9-0123-def0-456789012345"
      }
    ],
    "lowerBound": 1,
    "pageNo": 1,
    "pageSize": 10,
    "totalCount": 10,
    "totalPages": 1,
    "upperBound": 10
  },
  "success": true,
  "targetUrl": ""
}
```

## 22. Get Active Raffle

**POST** `/cms/raffles/get-all`

### Description

Gets all active raffles

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/cms/cms/raffles/get-all'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "createdAt": "2026-08-31 21:03:25.321459",
        "drawTime": "2026-08-31 23:03:00",
        "endsAt": "2026-08-31 22:02:00",
        "expectedRevenue": "50000.00",
        "id": "8990662a-e40f-4344-9b56-1fd31680aa64",
        "liability": "25000.00",
        "name": "tsest",
        "options": [
          {
            "id": "red",
            "name": "Red"
          },
          {
            "id": "gold",
            "name": "Gold"
          },
          {
            "id": "green",
            "name": "Green"
          },
          {
            "id": "black",
            "name": "Black"
          }
        ],
        "slug": "tsest-20260831",
        "startsAt": "2026-08-31 21:02:00",
        "status": 0,
        "totalEntries": 0
      }
    ]
  },
  "success": true,
  "targetUrl": ""
}

```


## 23. Get Raffle Options

**POST** `/cms/raffles/get-options`

### Description

Gets raffle options

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/cms/cms/raffles/get-options?raffleId=8990662a-e40f-4344-9b56-1fd31680aa64'
```

### Example Response

```json
{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "id": "red",
        "name": "Red"
      },
      {
        "id": "gold",
        "name": "Gold"
      },
      {
        "id": "green",
        "name": "Green"
      },
      {
        "id": "black",
        "name": "Black"
      }
    ]
  },
  "success": true,
  "targetUrl": ""
}

```

# ⚠️ Error Handling


## 24. Get Prize Categories

**POST** `/cms/prizes/get-all`

### Description

Gets All Prizes

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/prizes/get-all'
```

### Example Response

```json

{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "captionImageUrl": null,
        "code": "HYBRID",
        "createdAt": "2026-08-18 19:44:42.476657",
        "description": "Hybrid prize bundle",
        "id": "32059376-799d-4aed-be95-ff1c5a483acf",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Hybrid Prize",
        "priority": 3,
        "rewardType": 16
      },
      {
        "captionImageUrl": null,
        "code": "CONSOLATION",
        "createdAt": "2026-08-18 19:44:10.300180",
        "description": "Consolation prize",
        "id": "2db9f2f2-f8c3-4b06-af34-7a0d0aa574e0",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Consolation Prize",
        "priority": 3,
        "rewardType": 15
      },
      {
        "captionImageUrl": null,
        "code": "MEMBERSHIP",
        "createdAt": "2026-08-18 19:43:23.137915",
        "description": "Exclusive membership access",
        "id": "09cba6f4-2c92-4ada-b2d0-b4e357a79103",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Premium Club Membership",
        "priority": 3,
        "rewardType": 14
      },
      {
        "captionImageUrl": null,
        "code": "DATA_BUNDLE",
        "createdAt": "2026-08-18 19:42:53.043179",
        "description": "Data Bundles Across All Networks",
        "id": "2546805d-b520-4fe1-9bb3-ba777abb0211",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Data Bundle",
        "priority": 3,
        "rewardType": 13
      },
      {
        "captionImageUrl": null,
        "code": "AIRTIME",
        "createdAt": "2026-08-18 19:42:02.721801",
        "description": "Prepaid mobile airtime",
        "id": "0643c285-380f-4237-ace9-2427df760f84",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Mobile Airtime Top-up",
        "priority": 3,
        "rewardType": 12
      },
      {
        "captionImageUrl": null,
        "code": "DISCOUNT",
        "createdAt": "2026-08-18 19:39:53.165351",
        "description": "General Storewide discount",
        "id": "0b74b34f-532a-440d-a3c8-355415776bb1",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Discount",
        "priority": 3,
        "rewardType": 11
      },
      {
        "captionImageUrl": null,
        "code": "COUPON",
        "createdAt": "2026-08-18 19:39:08.072183",
        "description": "50% discount coupon",
        "id": "aea1857e-b2ab-4341-8e29-603d9d0836b4",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Coupon",
        "priority": 3,
        "rewardType": 10
      },
      {
        "captionImageUrl": null,
        "code": "DONATION",
        "createdAt": "2026-08-18 19:37:50.286484",
        "description": "Donation in winners name",
        "id": "6d5841cb-bcd6-4bac-89ac-328e8aee3789",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Donation",
        "priority": 3,
        "rewardType": 9
      },
      {
        "captionImageUrl": null,
        "code": "EXPERIENCE",
        "createdAt": "2026-08-18 19:32:21.097905",
        "description": "VIP experience at a concert",
        "id": "d5267836-5bd5-4c69-9088-7e84618d5aff",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Experience",
        "priority": 8,
        "rewardType": 8
      },
      {
        "captionImageUrl": null,
        "code": "SERVICE",
        "createdAt": "2026-08-18 19:23:28.200569",
        "description": "Local car wash service",
        "id": "ea7cad34-1ea4-4f53-97dd-a240fdff040b",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Service",
        "priority": 7,
        "rewardType": 7
      },
      {
        "captionImageUrl": null,
        "code": "SUBSCRIPTION",
        "createdAt": "2026-08-18 19:22:38.217284",
        "description": "Annual streaming subscription (Netflix, Youtube)",
        "id": "e24cbf3f-3ca7-4596-aba7-25bdec7013fc",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Subscription",
        "priority": 5,
        "rewardType": 6
      },
      {
        "captionImageUrl": null,
        "code": "DIGITAL_ITEM",
        "createdAt": "2026-08-18 19:21:42.010141",
        "description": "Digital Item",
        "id": "3af988b8-f81b-4727-b7d0-ecc67a2338c4",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Digital Items",
        "priority": 5,
        "rewardType": 5
      },
      {
        "captionImageUrl": null,
        "code": "PHYSICAL_ITEM",
        "createdAt": "2026-08-18 19:20:25.633292",
        "description": "Electronics prizes",
        "id": "7073c55d-99b8-4e82-92c6-fb6e1f5c33b2",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Physical Items",
        "priority": 4,
        "rewardType": 4
      },
      {
        "captionImageUrl": null,
        "code": "VOUCHER",
        "createdAt": "2026-08-18 19:16:50.048880",
        "description": "Relaxing spa day voucher",
        "id": "43c47481-1039-4953-879d-80ed0173ffa5",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Voucher",
        "priority": 2,
        "rewardType": 3
      },
      {
        "captionImageUrl": null,
        "code": "GIFT_CARD",
        "createdAt": "2026-08-18 19:14:31.533885",
        "description": "Digital Gift Card",
        "id": "8c35c0d6-5be4-46d4-8645-d6609e03e063",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Gift Card",
        "priority": 2,
        "rewardType": 2
      },
      {
        "captionImageUrl": null,
        "code": "POINTS_100",
        "createdAt": "2026-08-18 19:11:12.261763",
        "description": "Bonus loyalty points",
        "id": "80bfabea-b24c-43d1-b87d-cff0d79fb2d0",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 5,
        "name": "Loyalty Points Booster",
        "priority": 1,
        "rewardType": 1
      },
      {
        "captionImageUrl": null,
        "code": "CASH",
        "createdAt": "2026-08-18 19:07:39.520948",
        "description": "Ultimate cash prize",
        "id": "c6755cfd-63e5-4045-8d77-e49ea35d0768",
        "isActive": true,
        "isRepeatable": false,
        "maxWinners": 1,
        "name": "Grand Cash Prize",
        "priority": 0,
        "rewardType": 0
      }
    ],
    "lowerBound": 1,
    "pageNo": 1,
    "pageSize": 100,
    "totalCount": 17,
    "totalPages": 1,
    "upperBound": 17
  },
  "success": true,
  "targetUrl": ""
}

```

## 25. Get Draw History

**POST** `/cms/get-draw-history`

### Description

Gets Draw History

### Example Request

```bash
curl --location 'https://nsaa-api.rhyoliteprime.com/api/v1/get-draw-history'
```

### Example Response

```json

{
  "error": null,
  "message": "",
  "result": {
    "data": [
      {
        "drawTime": "2026-09-08 18:00:00",
        "id": "96ad67b2-abf9-4f46-b17c-018dfff56e03",
        "name": "Tuesday Boom Raffle ",
        "status": "DRAW COMPLETED",
        "winningOption": "Red"
      },
      {
        "drawTime": "2026-09-07 18:00:00",
        "id": "cd3da8e8-2c01-4430-8e45-447bfa3a4d3e",
        "name": "Super Monday Raffle",
        "status": "DRAW COMPLETED",
        "winningOption": "Green"
      },
      {
        "drawTime": "2026-09-07 17:00:00",
        "id": "2f5613e6-2ad9-4215-a9da-70381428a1b9",
        "name": "Old School Reunion Draw",
        "status": "DRAW COMPLETED",
        "winningOption": "Blue"
      },
      {
        "drawTime": "2026-09-04 18:45:00",
        "id": "de63caac-bacb-4f63-8c4c-e95b9fb0b84d",
        "name": "Mega Friday",
        "status": "DRAW_COMPLETED",
        "winningOption": "Green"
      },
      {
        "drawTime": "2026-09-03 19:00:00",
        "id": "66af3616-a59e-447d-b3f2-7f7132436c2f",
        "name": "Super Thursday",
        "status": "DRAW COMPLETED",
        "winningOption": "Green"
      },
      {
        "drawTime": "2026-09-02 17:30:00",
        "id": "2763d198-ed03-455e-9af9-2568e87bd501",
        "name": "Wednesday Action",
        "status": "DRAW COMPLETED",
        "winningOption": "Red"
      },
      {
        "drawTime": "2026-08-31 16:00:00",
        "id": "247a8687-055b-42ee-8a0d-ca8f1013c051",
        "name": "Monday Extra ",
        "status": "DRAW COMPLETED",
        "winningOption": "Gold"
      },
      {
        "drawTime": "2026-08-28 15:30:00",
        "id": "9badd3a5-2b19-4dd4-b9a4-fe20bbceb3e5",
        "name": "Super Friday Raffle",
        "status": "DRAW COMPLETED",
        "winningOption": "Red"
      },
      {
        "drawTime": "2026-08-26 23:15:00",
        "id": "a0af82ae-8b9b-4fc0-aad1-59baae8ccf29",
        "name": "Wednesday Raffle",
        "status": "DRAW COMPLETED",
        "winningOption": "Green"
      },
      {
        "drawTime": "2026-08-24 10:40:00",
        "id": "b4ff904c-7645-4892-8792-b8bb147f9826",
        "name": "Big Time Monday",
        "status": "CLOSED",
        "winningOption": ""
      }
    ],
    "lowerBound": 1,
    "pageNo": 1,
    "pageSize": 10,
    "totalCount": 11,
    "totalPages": 2,
    "upperBound": 10
  },
  "success": true,
  "targetUrl": ""
}

```


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
