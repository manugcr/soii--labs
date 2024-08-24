package controllers

import (
	"strconv"

	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/services"
	"github.com/gofiber/fiber/v2"
)

// @Summary Get available offers
// @Description Get all available offers
// @Tags auth
// @Accept json
// @Produce json
// @Param Authorization header string true "JWT <token>"
// @Security ApiKeyAuth
// @Success 200 {object} models.OffersResponse "offers"
// @Failure 401 {object} models.Response "Unauthorized"
// @Failure 500 {object} models.Response "Bad server"
// @Router /auth/offers [get]
func GetOffers(c *fiber.Ctx) error {
	offers, err := services.GetOffers()
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to get offers",
		})
	}

	return c.Status(fiber.StatusOK).JSON(models.OffersResponse{
		Code:    "200",
		Message: offers,
	})
}

// AddOffer adds a new offer to the database
// @Summary Add a new offer
// @Description Add a new offer to the available offers list
// @Tags auth
// @Accept json
// @Produce json
// @Param offer body models.Offer true "Offer"
// @Success 201 {object} models.Response "Offer added"
// @Failure 400 {object} models.Response "Invalid request payload"
// @Failure 500 {object} models.Response "Internal Server Error"
// @Router /auth/addoffer [post]
func AddOffer(c *fiber.Ctx) error {
	var offer models.Offer

	if err := c.BodyParser(&offer); err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(models.Response{
			Code:    "400",
			Message: "Invalid request payload",
		})
	}

	err := services.AddOffer(offer)
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to add offer",
		})
	}

	return c.Status(fiber.StatusCreated).JSON(models.Response{
		Code:    "201",
		Message: "Offer added successfully",
	})
}

// @Summary Checkout
// @Description Buy a list of orders.
// @Tags auth
// @Accept json
// @Produce json
// @Security ApiKeyAuth
// @Param checkout body models.CheckoutRequest true "Checkout Request"
// @Param Authorization header string true "JWT <token>"
// @Success 200 {object} models.CheckoutResponse
// @Failure 400 {object} models.Response "Bad request"
// @Failure 401 {object} models.Response "Unauthorized"
// @Failure 500 {object} models.Response "Bad server"
// @Router /auth/checkout [post]
func Checkout(c *fiber.Ctx) error {
	var request models.CheckoutRequest
	if err := c.BodyParser(&request); err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(models.Response{
			Code:    "400",
			Message: "Invalid request payload",
		})
	}

	// Process checkout
	orderID, total, err := services.Checkout(request)
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to checkout",
		})
	}

	return c.Status(fiber.StatusOK).JSON(models.CheckoutResponse{
		Code:    "200",
		Message: models.CheckoutMessage{Total: total, Status: "pending"},
		OrderID: orderID,
	})
}

// @Summary Get status of a specific order
// @Description Get the status of a specific order by id
// @Tags auth
// @Accept json
// @Produce json
// @Security ApiKeyAuth
// @Param id path string true "Order ID"
// @Param Authorization header string true "JWT <token>"
// @Success 200 {object} models.OrderStatusResponse "status"
// @Failure 401 {object} models.Response "Unauthorized"
// @Failure 500 {object} models.Response "Bad server"
// @Router /auth/orders/{id} [get]
func GetOrderStatus(c *fiber.Ctx) error {
	orderIDParam := c.Params("id")
	orderID, err := strconv.ParseUint(orderIDParam, 10, 32)
	if err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(models.Response{
			Code:    "400",
			Message: "Invalid order ID",
		})
	}

	order, err := services.GetOrderStatus(uint(orderID))
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to get order status",
		})
	}

	return c.Status(fiber.StatusOK).JSON(models.OrderStatusResponse{
		Code:    "200",
		Message: order.Status,
		Order:   order,
	})
}
