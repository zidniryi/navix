pub fn main() {
    println!("Hello, Rust!");
}

pub struct Point {
    x: f64,
    y: f64,
}

pub enum Color {
    Red,
    Blue,
}

pub trait Drawable {
    fn draw(&self);
}

impl Drawable for Point {
    fn draw(&self) {
        println!("Point at ({}, {})", self.x, self.y);
    }
}
