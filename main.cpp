#include <flecs.h>

#include <print>
#include <variant>
#include <string>

struct Child {};

struct Dirty {
    bool value;
};


struct StructuralElement {};

struct Point : StructuralElement {
    double x;
    double y;
};

struct PointBetweenTwoPoints : StructuralElement {};

struct PointOnLine : Point {
    double t;
};

struct Line : StructuralElement {
    double x0;
    double y0;

    double x1;
    double y2;
};

struct Surface {
    double area;
};

int main() {
    auto world = flecs::world{};

    world.import <flecs::units>();
    world.import <flecs::monitor>(); // Collect statistics periodically

    const flecs::entity &structuralElementComponent = world.component<StructuralElement>();
    const flecs::entity &childComponent = world.component<Child>().add(flecs::Traversable);
    const flecs::entity &pointComponent = world.component<Point>().is_a<StructuralElement>();
    const flecs::entity &lineComponent = world.component<Line>().is_a<StructuralElement>();
    const flecs::entity &pointBetweenTwoPointsComponent = world.component<PointBetweenTwoPoints>().is_a<StructuralElement>();
    const flecs::entity &pointOnLineComponent = world.component<PointOnLine>().is_a<StructuralElement>();

    //flecs::query<> q = world.query_builder().term<StructuralElement>().cascade(childComponent).self().term<Point>().inout(flecs::InOut).or_().term<Line>().build();

    //const flecs::entity &p1 = world.entity("P1").add<StructuralElement>().set<Point>({.x = 0.0, .y = 0.0}).set<Dirty>({ .value = true });
    //const flecs::entity &p2 = world.entity("P2").add<StructuralElement>().set<Point>({.x = 10.0, .y = 0.0}).set<Dirty>({.value = true});

    //const flecs::entity &line1 = world.entity("L1").add<StructuralElement>().set<Line>({}).add<Child>(p1).add<Child>(p2).set<Dirty>({.value = true});

    //const flecs::entity &p3 =
    //    world.entity("P3").add<StructuralElement>().set<Point>({.x = 0.0, .y = 0.0}).set<PointOnLine>({.t = 0.4}).add<Child>(p1).add<Child>(p2).set<Dirty>({.value = true});
    //const flecs::entity &p4 = world.entity("P4").add<StructuralElement>().set<Point>({.x = 5.0, .y = 5.0}).set<Dirty>({.value = true});

    //const flecs::entity &line2 = world.entity("L2").add<StructuralElement>().add<Line>().add<Child>(p3).add<Child>(p4).set<Dirty>({.value = true});

    const flecs::system calculatePointsPositionsSystem =
        world.system("CalculatePointPositions")
            .term<StructuralElement>()
            .cascade(childComponent)
            .self()
            .term<Point>()
            .inout(flecs::InOut)
            .or_()
            .term<Line>()
            .term<Dirty>()
            .each([&](flecs::iter &it, size_t index) {
                flecs::entity entity = it.entity(index);
                flecs::id term_id = it.id(2);        

                Dirty *dirty = entity.get_mut<Dirty>();
                dirty->value = false;

                if (term_id == world.id<Point>()) {
                    //std::println("Dirty {} ", dirty->value);
                    Point *v = entity.get_mut<Point>();

                    const PointOnLine *onLine = entity.get<PointOnLine>();
                    if (onLine) {
                        const Point *fromPoint = entity.target(childComponent, 0).get<Point>();
                        const Point *toPoint = entity.target(childComponent, 1).get<Point>();

                        auto newX = fromPoint->x * onLine->t + toPoint->x * (1.0 - onLine->t);
                        auto newY = fromPoint->y * onLine->t + toPoint->y * (1.0 - onLine->t);

                        if (newX != v->x || newY != v->y) {
                            *v = {.x = newX, .y = newY};
                            dirty->value = true;
                        }
                    }

                    /*std::println("Point {0} | X: {1:.3f} Y: {2:.3f}", entity.name().c_str(), v->x, v->y);*/
                }
                if (term_id == world.id<Line>()) {
                    // std::cout << "is a line " << entity.name() << std::endl;

                    flecs::entity from = entity.target(childComponent, 0);
                    flecs::entity to = entity.target(childComponent, 1);
                    if (from && to) {
                        auto fromPoint = from.get<Point>();
                        auto toPoint = to.get<Point>();
                    }
                }
            });

    world.system<const Point, const Dirty>("PrintPoints").each([](const Point& p, const Dirty& dirty) {
        if (dirty.value) {
            std::println("Point X: {0:.3f} Y: {1:.3f}", p.x, p.y);
        }
    });

    for (int i = 0; i < 100000; i++) {
        const flecs::entity &p = world.entity().add<StructuralElement>().set<Point>({.x = 0.0, .y = 0.0}).set<Dirty>({.value = true});
    }

    world.progress();
    world.progress();
    world.progress();

    return world.app().enable_rest().run();
}
