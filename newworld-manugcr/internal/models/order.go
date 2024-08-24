package models

type UpdateOrderStatusRequest struct {
	Status string `json:"status"`
}

type OrderStatus struct {
	Status string `json:"status"`
}
