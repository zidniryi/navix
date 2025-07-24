package main

import (
	"encoding/json"
	"fmt"
	"net/http"
)

// User represents a user in the system
type User struct {
	ID    int    `json:"id"`
	Name  string `json:"name"`
	Email string `json:"email"`
}

// UserService defines user operations
type UserService interface {
	GetUser(id int) (*User, error)
	CreateUser(user User) error
	UpdateUser(user User) error
	DeleteUser(id int) error
}

// Config holds application configuration
type Config struct {
	Port     int
	Database string
	Debug    bool
}

// ValidationError represents a validation error
type ValidationError struct {
	Message string
}

// Error implements the error interface
func (v ValidationError) Error() string {
	return v.Message
}

const (
	DefaultPort = 8080
	MaxUsers    = 1000
	AppName     = "GoApp"
)

var (
	globalConfig Config
	userCount    int
	isRunning    bool
)

// NewConfig creates a new configuration
func NewConfig() *Config {
	return &Config{
		Port:     DefaultPort,
		Database: "postgres://localhost/app",
		Debug:    false,
	}
}

// GetFullName returns the full display name
func (u User) GetFullName() string {
	return fmt.Sprintf("%s <%s>", u.Name, u.Email)
}

// SetEmail updates the user's email
func (u *User) SetEmail(email string) error {
	if email == "" {
		return ValidationError{Message: "email cannot be empty"}
	}
	u.Email = email
	return nil
}

// IsValid checks if config is valid
func (c *Config) IsValid() bool {
	return c.Port > 0 && c.Database != ""
}

// HandleUsers processes HTTP requests for users
func HandleUsers(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case "GET":
		users := []User{
			{ID: 1, Name: "John Doe", Email: "john@example.com"},
			{ID: 2, Name: "Jane Smith", Email: "jane@example.com"},
		}
		json.NewEncoder(w).Encode(users)
	case "POST":
		var user User
		json.NewDecoder(r.Body).Decode(&user)
		fmt.Fprintf(w, "Created user: %s", user.Name)
	default:
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
	}
}

// processData performs data processing
func processData(data []byte) (result map[string]interface{}, err error) {
	err = json.Unmarshal(data, &result)
	return
}

// validateInput checks input validity
func validateInput(input string) bool {
	return len(input) > 0 && len(input) < 100
}

// main is the entry point
func main() {
	config := NewConfig()
	globalConfig = *config

	// Short variable declarations
	router := http.NewServeMux()
	server := &http.Server{
		Addr:    fmt.Sprintf(":%d", config.Port),
		Handler: router,
	}

	// Register handlers
	router.HandleFunc("/users", HandleUsers)
	router.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, "OK")
	})

	// Start server
	fmt.Printf("Starting %s server on port %d\n", AppName, config.Port)
	if err := server.ListenAndServe(); err != nil {
		fmt.Printf("Server error: %v\n", err)
	}
}
