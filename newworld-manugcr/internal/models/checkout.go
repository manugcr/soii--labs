package models

type Order struct {
	ID         uint        `json:"id"`
	Status     string      `json:"status"`
	Total      int         `json:"total"`
	OrderItems []OrderItem `gorm:"foreignKey:OrderID"`
}

type OrderItem struct {
	ID        uint `json:"id" example:"1"`
	OrderID   uint `json:"orderID" example:"1"`
	ProductID uint `json:"productID" example:"1"`
	Quantity  int  `json:"quantity" example:"10"`
	Price     int  `json:"price" example:"100"`
}

type OrderItemRequest struct {
	ProductID uint `json:"productID" example:"1"`
	Quantity  int  `json:"quantity" example:"10"`
}

type CheckoutRequest struct {
	OrderItems []OrderItemRequest `json:"orderItems"`
}

type CheckoutMessage struct {
	Total  int    `json:"total"`
	Status string `json:"status"`
}
