#include <flecs.h>
#include <iostream>
#include <print>

struct StructuralElement {};
struct Point : StructuralElement {
    double x;
    double y;
};
struct Line : StructuralElement {
    double x0;
    double y0;

    double x1;
    double y2;
};
struct PointBetweenTwoPoints : StructuralElement {};
struct PointOnLine : Point {
    double t;
};

struct Child {};

int main() {
    auto world = flecs::world{};

    world.component<StructuralElement>();
    const flecs::entity &pointComponent = world.component<Point>().is_a<StructuralElement>();
    const flecs::entity &lineComponent = world.component<Line>().is_a<StructuralElement>();
    const flecs::entity &pointBetweenTwoPointsComponent = world.component<PointBetweenTwoPoints>().is_a<StructuralElement>();
    const flecs::entity &pointOnLineComponent = world.component<PointOnLine>().is_a<StructuralElement>();

    const flecs::entity &childComponent = world.component<Child>().add(flecs::Traversable);

    const flecs::entity &p1 = world.entity("P1").add<StructuralElement>().set<Point>({.x = 0.0, .y = 0.0});
    const flecs::entity &p2 = world.entity("P2").add<StructuralElement>().set<Point>({.x = 10.0, .y = 0.0});

    const flecs::entity &line1 = world.entity("L1").add<StructuralElement>().set<Line>({}).add<Child>(p1).add<Child>(p2);

    const flecs::entity &p3 = world.entity("P3").add<StructuralElement>().set<Point>({.x = 0.0, .y = 0.0}).set<PointOnLine>({.t = 0.4}).add<Child>(p1).add<Child>(p2);
    const flecs::entity &p4 = world.entity("P4").add<StructuralElement>().set<Point>({.x = 5.0, .y = 5.0});

    const flecs::entity &line2 = world.entity("L2").add<StructuralElement>().add<Line>().add<Child>(p3).add<Child>(p4);

    flecs::query<> q = world.query_builder().term<StructuralElement>().cascade(childComponent).self().term<Point>().inout(flecs::InOut).or_().term<Line>().build();

    q.each([&](flecs::iter &it, size_t index) {
        flecs::entity entity = it.entity(index);
        flecs::id term_id = it.id(2);

        if (term_id == world.id<Point>()) {
            Point *v = entity.get_mut<Point>();

            const PointOnLine *onLine = entity.get<PointOnLine>();
            if (onLine) {
                const Point *fromPoint = entity.target(childComponent, 0).get<Point>();
                const Point *toPoint = entity.target(childComponent, 1).get<Point>();

                *v = {.x = fromPoint->x * onLine->t + toPoint->x * (1.0 - onLine->t), .y = fromPoint->y * onLine->t + toPoint->y * (1.0 - onLine->t)};
            }

            std::println("Point {0} | X: {1:.3f} Y: {2:.3f}", entity.name().c_str(), v->x, v->y);
        }
        if (term_id == world.id<Line>()) {
            std::cout << "is a line " << entity.name() << std::endl;

            flecs::entity from = entity.target(childComponent, 0);
            flecs::entity to = entity.target(childComponent, 1);
            if (from && to) {
                auto fromPoint = from.get<Point>();
                auto toPoint = to.get<Point>();
            }
        }
    });

    return 0;
}
