package main

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
	"os"

	_ "github.com/ICOMP-UNC/newworld-manugcr/docs"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/database"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/routes"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/cors"
	"github.com/gofiber/swagger"
	"gorm.io/gorm"
)

type Supplies struct {
	Food struct {
		Fruits     int `json:"fruits"`
		Meat       int `json:"meat"`
		Vegetables int `json:"vegetables"`
		Water      int `json:"water"`
	} `json:"food"`
	Medicine struct {
		Analgesics  int `json:"analgesics"`
		Antibiotics int `json:"antibiotics"`
		Bandages    int `json:"bandages"`
	} `json:"medicine"`
}

// fetchData fetches the supplies data from the server.cpp endpoint
func fetchData(url string) (*Supplies, error) {
	resp, err := http.Get(url)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}

	var supplies Supplies
	if err := json.Unmarshal(body, &supplies); err != nil {
		return nil, err
	}
	return &supplies, nil
}

// saveSuppliesToDB saves the supplies data to the database as offers
func saveSuppliesToDB(db *gorm.DB, supplies *Supplies) error {
	offers := []models.Offer{
		{Name: "fruits", Category: "food", Quantity: supplies.Food.Fruits, Price: 10},
		{Name: "meat", Category: "food", Quantity: supplies.Food.Meat, Price: 20},
		{Name: "vegetables", Category: "food", Quantity: supplies.Food.Vegetables, Price: 15},
		{Name: "water", Category: "food", Quantity: supplies.Food.Water, Price: 5},
		{Name: "analgesics", Category: "medicine", Quantity: supplies.Medicine.Analgesics, Price: 50},
		{Name: "antibiotics", Category: "medicine", Quantity: supplies.Medicine.Antibiotics, Price: 100},
		{Name: "bandages", Category: "medicine", Quantity: supplies.Medicine.Bandages, Price: 30},
	}

	for _, offer := range offers {
		if err := db.Create(&offer).Error; err != nil {
			return err
		}
	}
	return nil
}

// @title Fiber Example API
// @version 1.0
// @description This is a sample Swagger for Fiber
// @termsOfService http://swagger.io/terms/
// @contact.name API Support
// @contact.email fiber@swagger.io
// @license.name Apache 2.0
// @license.url http://www.apache.org/licenses/LICENSE-2.0.html
// @host localhost:3000
// @BasePath /
func main() {
	// Reset the database to start fresh
	database.ResetDatabase()

	// Create a new Fiber instance
	app := fiber.New()

	// Enable CORS
	app.Use(cors.New())

	// Initialize the database
	db := database.InitDB()

	// Fetch and save supplies data
	suppliesURL := os.Getenv("SUPPLIES_URL")
	supplies, err := fetchData(suppliesURL)
	if err != nil {
		log.Fatalf("Failed to fetch supplies data: %v", err)
	}
	if err := saveSuppliesToDB(db, supplies); err != nil {
		log.Fatalf("Failed to save supplies data to database: %v", err)
	}

	// Register the endpoints
	routes.SetupRoutes(app)

	// Register the Swagger middleware
	app.Get("/swagger/*", swagger.HandlerDefault)

	// Start the server on port 3000
	log.Fatal(app.Listen(":3000"))
}
