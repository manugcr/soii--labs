package repositories

import (
	"fmt"

	"github.com/ICOMP-UNC/newworld-manugcr/internal/database"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
)

func FindOffers() ([]models.Offer, error) {
	var offers []models.Offer
	if err := database.DB.Find(&offers).Error; err != nil {
		return nil, err
	}
	return offers, nil
}

func FindOrder(orderID uint) (models.Order, error) {
	var order models.Order
	if err := database.DB.Preload("OrderItems").First(&order, orderID).Error; err != nil {
		return models.Order{}, err
	}
	return order, nil
}

func CreateOffer(offer models.Offer) error {
	return database.DB.Create(&offer).Error
}

func CreateOrder(request models.CheckoutRequest) (uint, int, error) {
	var total int
	var orderItems []models.OrderItem

	// Start a database transaction
	tx := database.DB.Begin()
	if tx.Error != nil {
		return 0, 0, tx.Error
	}

	// Calculate total price and prepare order items
	for _, item := range request.OrderItems {
		var offer models.Offer
		if err := tx.First(&offer, item.ProductID).Error; err != nil {
			tx.Rollback()
			return 0, 0, err
		}

		// Check if the requested quantity is negative
		if item.Quantity <= 0 {
			tx.Rollback()
			return 0, 0, fmt.Errorf("invalid quantity for product %d", item.ProductID)
		}

		// Check if there are enough items in stock
		if item.Quantity > offer.Quantity {
			tx.Rollback()
			return 0, 0, fmt.Errorf("not enough items in stock for product %d", item.ProductID)
		}

		total += offer.Price * item.Quantity
		orderItems = append(orderItems, models.OrderItem{
			ProductID: item.ProductID,
			Quantity:  item.Quantity,
			Price:     offer.Price,
		})
	}

	// Create the order
	order := models.Order{
		Status:     "pending",
		Total:      total,
		OrderItems: orderItems,
	}

	if err := tx.Create(&order).Error; err != nil {
		tx.Rollback()
		return 0, 0, err
	}

	// Update offer quantities
	for _, item := range request.OrderItems {
		var offer models.Offer
		if err := tx.First(&offer, item.ProductID).Error; err != nil {
			tx.Rollback()
			return 0, 0, err
		}
		offer.Quantity -= item.Quantity
		if err := tx.Save(&offer).Error; err != nil {
			tx.Rollback()
			return 0, 0, err
		}
	}

	// Update the market balance
	if err := UpdateMarketBalance(-total); err != nil {
		tx.Rollback()
		return 0, 0, err
	}

	// Commit the transaction
	if err := tx.Commit().Error; err != nil {
		return 0, 0, err
	}

	return order.ID, total, nil
}
