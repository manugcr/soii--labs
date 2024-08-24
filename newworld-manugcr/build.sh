echo " > Tidying..."
go mod tidy

echo " > Setting up swagger..."
swag init -g cmd/main.go -o ./docs --parseDependency

echo " > Running..."
go run cmd/main.go
