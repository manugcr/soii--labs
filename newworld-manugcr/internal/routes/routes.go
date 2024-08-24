package routes

import (
	"github.com/ICOMP-UNC/newworld-manugcr/internal/controllers"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/middleware"
	"github.com/gofiber/fiber/v2"
)

func SetupRoutes(app *fiber.App) {
	app.Post("/auth/addoffer", controllers.AddOffer)                                        // Add an offer (debug only)
	app.Post("/auth/register", controllers.RegisterUser)                                    // Register a new user
	app.Post("/auth/login", controllers.LoginUser)                                          // Authenticate a user
	app.Get("/auth/offers", middleware.ValidateJWT(), controllers.GetOffers)                // Retrieve a list of available offers
	app.Post("/auth/checkout", middleware.ValidateJWT(), controllers.Checkout)              // Buy a list of orders
	app.Get("/auth/orders/:id", middleware.ValidateJWT(), controllers.GetOrderStatus)       // Get the status of an order
	app.Get("/admin/dashboard", middleware.ValidateJWT(), controllers.GetDashboard)         // Get the status of all market
	app.Patch("/admin/orders/:id", middleware.ValidateJWT(), controllers.UpdateOrderStatus) // Update the status of an order
}
