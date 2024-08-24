package services

import (
	"strconv"

	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/repositories"
)

func GetDashboardData() (models.DashboardData, error) {
	offers, err := repositories.FindOffers()
	if err != nil {
		return models.DashboardData{}, err
	}

	orders, err := repositories.FindOrders()
	if err != nil {
		return models.DashboardData{}, err
	}

	balance, err := repositories.GetMarketBalance()
	if err != nil {
		return models.DashboardData{}, err
	}

	return models.DashboardData{
		Offers:  offers,
		Orders:  orders,
		Balance: balance,
	}, nil
}

func UpdateOrderStatus(orderIDStr string, status string) error {
	orderID, err := strconv.Atoi(orderIDStr)
	if err != nil {
		return err
	}

	return repositories.UpdateOrderStatus(uint(orderID), status)
}
