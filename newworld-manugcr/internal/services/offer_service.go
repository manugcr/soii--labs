package services

import (
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/repositories"
)

func GetOffers() ([]models.Offer, error) {
	offers, err := repositories.FindOffers()
	if err != nil {
		return nil, err
	}
	return offers, nil
}

func AddOffer(offer models.Offer) error {
	return repositories.CreateOffer(offer)
}

func Checkout(request models.CheckoutRequest) (uint, int, error) {
	// Calculate total price and create order
	orderID, total, err := repositories.CreateOrder(request)
	if err != nil {
		return 0, 0, err
	}
	return orderID, total, nil
}

func GetOrderStatus(orderID uint) (models.Order, error) {
	order, err := repositories.FindOrder(orderID)
	if err != nil {
		return models.Order{}, err
	}
	return order, nil
}
