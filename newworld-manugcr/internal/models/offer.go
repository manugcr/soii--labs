package models

type Offer struct {
	ID       uint   `gorm:"primaryKey" json:"id" example:"1"`
	Name     string `json:"name" example:"meat"`
	Quantity int    `json:"quantity" example:"10"`
	Price    int    `json:"price" example:"100"`
	Category string `json:"category" example:"food"`
}

type OffersRequest struct {
	Token string `json:"token"`
}
