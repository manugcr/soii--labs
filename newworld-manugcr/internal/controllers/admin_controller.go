package controllers

import (
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/services"
	"github.com/gofiber/fiber/v2"
)

// @Summary Get status of all market
// @Description Get status of all market offers, orders, and balance
// @Tags admin
// @Accept json
// @Produce json
// @Security ApiKeyAuth
// @Param Authorization header string true "JWT <token>"
// @Success 200 {object} models.DashboardResponse "dashboard"
// @Failure 401 {object} models.Response "Unauthorized"
// @Failure 500 {object} models.Response "Bad server"
// @Router /admin/dashboard [get]
func GetDashboard(c *fiber.Ctx) error {
	dashboardData, err := services.GetDashboardData()
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to get dashboard data",
		})
	}

	return c.Status(fiber.StatusOK).JSON(models.DashboardResponse{
		Code:    "200",
		Message: dashboardData,
	})
}

// @Summary Update status of a specific order
// @Description Update the status of a specific order by id
// @Tags admin
// @Accept json
// @Produce json
// @Security ApiKeyAuth
// @Param id path string true "Order ID"
// @Param Authorization header string true "JWT <token>"
// @Param status body models.UpdateOrderStatusRequest true "Order Status"
// @Success 200 {object} models.UpdateOrderStatusResponse "status"
// @Failure 401 {object} models.Response "Unauthorized"
// @Failure 500 {object} models.Response "Bad server"
// @Router /admin/orders/{id} [patch]
func UpdateOrderStatus(c *fiber.Ctx) error {
	orderID := c.Params("id")
	var request models.UpdateOrderStatusRequest

	if err := c.BodyParser(&request); err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(models.Response{
			Code:    "400",
			Message: "Invalid request payload",
		})
	}

	err := services.UpdateOrderStatus(orderID, request.Status)
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to update order status",
		})
	}

	return c.Status(fiber.StatusOK).JSON(models.UpdateOrderStatusResponse{
		Code:    "200",
		Message: models.OrderStatus{Status: request.Status},
	})
}
