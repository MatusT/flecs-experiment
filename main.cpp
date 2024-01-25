#include <flecs.h>

#include <glm/ext/matrix_clip_space.hpp> // perspective
#include <glm/ext/matrix_transform.hpp>  // translate, rotate
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include <print>
#include <string>
#include <variant>
#include <vector>

struct Child
{};

struct Position
{
    glm::vec3 value;
};

struct StructuralElement
{};

struct Node : StructuralElement
{};

struct NodeOnLine : Node
{
    float t;
};

struct Line : StructuralElement
{
    glm::vec3 from;
    glm::vec3 to;
};

struct Surface : StructuralElement
{
    double area;
};

struct NodalSupportType
{};

struct Member
{};

struct OnLine
{};

struct HasHinge
{};

struct HasSupport
{};

struct HingeType
{};

struct SupportType
{};

struct Lol
{
    float a;
};

int
main()
{
    auto world = flecs::world{};

    world.import <flecs::units>();
    world.import <flecs::monitor>(); // Collect statistics periodically

    const flecs::entity& structuralElementComponent = world.component<StructuralElement>();
    const flecs::entity& childComponent = world.component<Child>().add(flecs::Traversable);
    const flecs::entity& pointComponent = world.component<Node>().is_a<StructuralElement>();
    const flecs::entity& lineComponent = world.component<Line>().is_a<StructuralElement>();
    world.component<Position>();
    const flecs::entity& surfaceComponent = world.component<Surface>().is_a<StructuralElement>();
    const flecs::entity& pointOnLineComponent = world.component<NodeOnLine>().is_a<StructuralElement>();

    const flecs::entity& hingeTypeComponent = world.component<HingeType>();

    const flecs::entity& hasHingeComponent = world.component<HasHinge>().add(flecs::Exclusive);
    const flecs::entity& hasSupportComponent = world.component<HasSupport>().add(flecs::Exclusive);
    const flecs::entity& memberComponent = world.component<Member>();

    auto rebuildStructure = world.system("RebuildStructure").kind(flecs::OnUpdate).expr("StructuralElement(self|up(Child))").each([&](const flecs::entity& entity) {
        // std::println("Name {}", entity.name().c_str());

        if (entity.has<NodeOnLine>()) {
            NodeOnLine* nodeOnLine = entity.get_mut<NodeOnLine>();

            const Line* line = entity.target<Child>(0).get<Line>();

            if (line) {
                glm::vec3 v = glm::normalize(line->from - line->to);

                glm::vec3 position = line->from + nodeOnLine->t * v;

                // std::println("Node on line position {}", position.x);
            }
        }

        if (entity.has<Line>()) {
            Line* line = entity.get_mut<Line>();

            const Position* fromPosition = entity.target<Child>(0).get<Position>();
            const Position* toPosition = entity.target<Child>(1).get<Position>();

            if (fromPosition && toPosition) {
                line->from = fromPosition->value;
                line->to = toPosition->value;

                // std::println("Line from {} to {}", line->from, line->to);
            }
        }

        if (entity.has<Surface>()) {
        }
    });
    flecs::query<const Node, const Position> collectNodes = world.query<const Node, const Position>();

    const flecs::entity& p1 = world.entity("P1").add<Node>().set<Position>({ .value = glm::vec3(0.0) });
    const flecs::entity& p2 = world.entity("P2").add<Node>().set<Position>({ .value = glm::vec3(1.0, 0.0, 0.0) });

    const flecs::entity& line1 = world.entity("L1").add<StructuralElement>().set<Line>({}).add<Child>(p1).add<Child>(p2);

    const flecs::entity& p3 = world.entity("P3").set<NodeOnLine>({ .t = 0.4 }).add<Child>(p1).add<Child>(p2);
    const flecs::entity& p4 = world.entity("P4").add<Node>().set<Position>({ .value = glm::vec3(5.0, 5.0, 0.0) });

    const flecs::entity& line2 = world.entity("L2").add<Line>().add<Child>(p3).add<Child>(p4);

    world.progress();

    std::vector<glm::vec3> nodes{};
    collectNodes.iter([&](const flecs::iter& it, const Node* node, const Position* position) {
        std::println("TABLE");
        for (auto i : it) {
            std::println("{} {} {}", position[i].value.x, position[i].value.y, position[i].value.z);
             nodes.push_back(position[i].value);
        }
    });
    // world.system<const Position, const Dirty>("PrintPoints").each([](const Position &p, const Dirty &dirty) {
    //     if (dirty.value) {
    //         std::println("Point X: {0:.3f} Y: {1:.3f}", p.x, p.y);
    //     }
    // });

    // for (int i = 0; i < 100000; i++) {
    //     const flecs::entity &p = world.entity().add<StructuralElement>().set<Point>({.x = 0.0, .y = 0.0});
    // }

    return 0;
}
