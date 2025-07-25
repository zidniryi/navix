import Foundation
import UIKit

// MARK: - Protocol Definition
protocol Drawable {
    func draw()
    var area: Double { get }
}

// MARK: - Class Definition  
class Shape: Drawable {
    var name: String
    var color: UIColor
    
    init(name: String, color: UIColor) {
        self.name = name
        self.color = color
    }
    
    func draw() {
        print("Drawing \(name) with color \(color)")
    }
    
    var area: Double {
        return 0.0
    }
    
    deinit {
        print("Shape \(name) is being deallocated")
    }
}

// MARK: - Struct Definition
struct Rectangle: Drawable {
    let width: Double
    let height: Double
    var fillColor: UIColor = .blue
    
    var area: Double {
        return width * height
    }
    
    func draw() {
        print("Drawing rectangle: \(width) x \(height)")
    }
    
    subscript(dimension: String) -> Double {
        switch dimension {
        case "width": return width
        case "height": return height
        default: return 0.0
        }
    }
}

// MARK: - Enum Definition
enum GeometryError: Error {
    case invalidDimension
    case negativeValue(Double)
    case unknownShape(String)
    
    var localizedDescription: String {
        switch self {
        case .invalidDimension:
            return "Invalid dimension provided"
        case .negativeValue(let value):
            return "Negative value not allowed: \(value)"
        case .unknownShape(let shape):
            return "Unknown shape: \(shape)"
        }
    }
}

// MARK: - Extension
extension Rectangle {
    func perimeter() -> Double {
        return 2 * (width + height)
    }
    
    var isSquare: Bool {
        return width == height
    }
    
    static func square(side: Double) -> Rectangle {
        return Rectangle(width: side, height: side)
    }
}

// MARK: - Functions
func calculateArea(of shapes: [Drawable]) -> Double {
    return shapes.reduce(0) { total, shape in
        return total + shape.area
    }
}

func processGeometry() throws {
    let rectangle = Rectangle(width: 10.0, height: 5.0)
    let square = Rectangle.square(side: 7.0)
    
    guard rectangle.width > 0 else {
        throw GeometryError.negativeValue(rectangle.width)
    }
    
    print("Rectangle area: \(rectangle.area)")
    print("Square area: \(square.area)")
    print("Rectangle perimeter: \(rectangle.perimeter())")
}

// MARK: - Generic Functions
func findMax<T: Comparable>(_ items: [T]) -> T? {
    return items.max()
}

func swap<T>(_ a: inout T, _ b: inout T) {
    let temp = a
    a = b
    b = temp
}

// MARK: - Constants and Variables
let π = 3.14159
var currentTemperature: Double = 25.0
let supportedShapes = ["circle", "rectangle", "triangle"]

// MARK: - Computed Properties
var temperatureInFahrenheit: Double {
    get {
        return currentTemperature * 9/5 + 32
    }
    set {
        currentTemperature = (newValue - 32) * 5/9
    }
}

// MARK: - Class with Subscripts
class Matrix {
    private var data: [[Double]]
    let rows: Int
    let columns: Int
    
    init(rows: Int, columns: Int) {
        self.rows = rows
        self.columns = columns
        self.data = Array(repeating: Array(repeating: 0.0, count: columns), count: rows)
    }
    
    subscript(row: Int, column: Int) -> Double {
        get {
            return data[row][column]
        }
        set {
            data[row][column] = newValue
        }
    }
}

// MARK: - Advanced Swift Features
class ViewControllerManager {
    private var viewControllers: [UIViewController] = []
    
    func addViewController(_ vc: UIViewController) {
        viewControllers.append(vc)
    }
    
    var isEmpty: Bool {
        return viewControllers.isEmpty
    }
    
    subscript(index: Int) -> UIViewController? {
        guard index >= 0 && index < viewControllers.count else {
            return nil
        }
        return viewControllers[index]
    }
}

// MARK: - Main execution
func main() {
    do {
        try processGeometry()
    } catch let error as GeometryError {
        print("Geometry error: \(error.localizedDescription)")
    } catch {
        print("Unknown error: \(error)")
    }
} 