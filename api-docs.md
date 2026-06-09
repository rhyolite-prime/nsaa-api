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
