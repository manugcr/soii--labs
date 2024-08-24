package repositories

import (
	"github.com/ICOMP-UNC/newworld-manugcr/internal/database"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
)

// CreateUser creates a new user in the database.
func CreateUser(user *models.User) error {
	return database.DB.Create(user).Error
}

// FindUserByEmail retrieves a user by their email.
func FindUserByEmail(email string) (*models.User, error) {
	var user models.User
	err := database.DB.Where("email = ?", email).First(&user).Error
	return &user, err
}
