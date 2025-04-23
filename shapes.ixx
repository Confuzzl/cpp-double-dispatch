export module shapes;

import dispatcher;

export {
  struct Shape {
    unsigned long long type;

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
}