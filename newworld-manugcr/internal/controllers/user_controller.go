package controllers

import (
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/services"
	"github.com/gofiber/fiber/v2"
)

// RegisterUser registers a new user.
// @Summary Register a new user
// @Description Register a new user with the given details
// @Tags auth
// @Accept json
// @Produce json
// @Param user body models.UserRegisterRequest true "User"
// @Success 201 {object} models.Response "User added"
// @Failure 400 {object} models.Response "Bad request"
// @Failure 500 {object} models.Response "Bad server"
// @Router /auth/register [post]
func RegisterUser(c *fiber.Ctx) error {
	var userReq models.UserRegisterRequest

	if err := c.BodyParser(&userReq); err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(models.Response{
			Code:    "400",
			Message: "Invalid request payload",
		})
	}

	err := services.RegisterUser(userReq)
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to register user",
		})
	}

	return c.Status(fiber.StatusCreated).JSON(models.Response{
		Code:    "201",
		Message: "User added",
	})
}

// LoginUser logs in a user and returns a JWT.
// @Summary Login a user
// @Description Login a user with the given credentials
// @Tags auth
// @Accept json
// @Produce json
// @Param login body models.UserLoginRequest true "Login Request"
// @Success 200 {object} models.LoginResponse "JWT token"
// @Failure 400 {object} models.Response "Bad request"
// @Failure 500 {object} models.Response "Bad server"
// @Router /auth/login [post]
func LoginUser(c *fiber.Ctx) error {
	var loginReq models.UserLoginRequest

	if err := c.BodyParser(&loginReq); err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(models.Response{
			Code:    "400",
			Message: "Invalid request payload",
		})
	}

	token, err := services.LoginUser(loginReq)
	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(models.Response{
			Code:    "500",
			Message: "Failed to login user",
		})
	}

	return c.Status(fiber.StatusOK).JSON(models.LoginResponse{
		Code: "201",
		Auth: token,
	})
}
