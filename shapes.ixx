export module shapes;

import dispatcher;

export {
  struct Shape {
    TypeSizeType type;

    template <typename T> Shape(const T *);
  };
  struct Wall : Shape {
    Wall() : Shape(this) {}
  };
  struct Circle : Shape {
    Circle() : Shape(this) {}
  };
  struct Polygon : Shape {
    Polygon() : Shape(this) {}
  };

  using ShapeTypes = TypeRegistrar<Wall, Circle, Polygon>;
  template <typename T>
  Shape::Shape(const T *) : type{ShapeTypes::getIndex<T>()} {}

  template <> TypeSizeType getType<Shape>(const Shape &s) { return s.type; }
}