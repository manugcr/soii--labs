package services

import (
	"errors"

	"github.com/ICOMP-UNC/newworld-manugcr/internal/middleware"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/models"
	"github.com/ICOMP-UNC/newworld-manugcr/internal/repositories"
	"golang.org/x/crypto/bcrypt"
)

func RegisterUser(userReq models.UserRegisterRequest) error {
	var role string

	// If username is ubuntu, set role to admin
	if userReq.Username == "ubuntu" {
		role = "admin"
	} else {
		role = "user"
	}

	// Hash the password using bcrypt
	hashedPassword, err := bcrypt.GenerateFromPassword([]byte(userReq.Password), bcrypt.DefaultCost)
	if err != nil {
		return err
	}

	user := models.User{
		Username: userReq.Username,
		Email:    userReq.Email,
		Password: string(hashedPassword), // Store the hashed password
		Role:     role,
		// No need to store the token
	}
	return repositories.CreateUser(&user)
}

func LoginUser(loginReq models.UserLoginRequest) (string, error) {
	user, err := repositories.FindUserByEmail(loginReq.Email)
	if err != nil {
		return "", err
	}

	// Compare the hashed password with the provided password
	err = bcrypt.CompareHashAndPassword([]byte(user.Password), []byte(loginReq.Password))
	if err != nil {
		return "", errors.New("incorrect password")
	}

	// Generate JWT token dynamically on login
	token, err := middleware.GenerateJWT(user.Email, user.Role)
	if err != nil {
		return "", err
	}

	return token, nil
}
