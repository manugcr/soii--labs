package models

type Response struct {
	Code    string `json:"code"`
	Message string `json:"message"`
}

type RegisterResponse struct {
	Username string `json:"username"`
	Token    string `json:"token"`
}

type UsersResponse struct {
	Code  string `json:"code"`
	Users []User `json:"users"`
}

type LoginResponse struct {
	Code string `json:"code"`
	Auth string `json:"auth"`
}

type OffersResponse struct {
	Code    string  `json:"code"`
	Message []Offer `json:"message"`
}

type CheckoutResponse struct {
	Code    string          `json:"code"`
	Message CheckoutMessage `json:"message"`
	OrderID uint            `json:"order_id"`
}

type OrderStatusResponse struct {
	Code    string `json:"code"`
	Message string `json:"message"`
	Order   Order  `json:"order"`
}

type UpdateOrderStatusResponse struct {
	Code    string      `json:"code"`
	Message OrderStatus `json:"message"`
}
