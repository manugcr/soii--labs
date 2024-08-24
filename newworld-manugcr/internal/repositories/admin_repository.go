package repositories

import (
	"sync"

	"github.com/ICOMP-UNC/newworld-manugcr/internal/database"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
)

var (
	marketBalance = 5000
	mu            sync.Mutex
)

func GetMarketBalance() (int, error) {
	mu.Lock()
	defer mu.Unlock()
	return marketBalance, nil
}

func UpdateMarketBalance(amount int) error {
	// Lock and update balance if you're using an in-memory approach
	mu.Lock()
	defer mu.Unlock()
	marketBalance += amount
	return nil
}

func FindOrders() ([]models.Order, error) {
	var orders []models.Order
	if err := database.DB.Preload("OrderItems").Find(&orders).Error; err != nil {
		return nil, err
	}
	return orders, nil
}

func UpdateOrderStatus(orderID uint, status string) error {
	return database.DB.Model(&models.Order{}).Where("id = ?", orderID).Update("status", status).Error
}
