#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"

#include "edm4hep/VertexCollection.h"
#include "edm4hep/VertexRecoParticleLinkCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "podio/CollectionBase.h"

TEST_CASE("Move Collections", "[Move Collections]") {
  SECTION("Vertex move") {
    edm4hep::VertexCollection col;
    { auto v = col.create(); }
    REQUIRE(col.size() == 1);

    edm4hep::VertexCollection col2 = std::move(col);
    REQUIRE(col2.size() == 1);
    REQUIRE(col.size() == 0);
  }
  SECTION("ReconstructedParticle move") {
    edm4hep::ReconstructedParticleCollection col;
    { auto v = col.create(); }
    REQUIRE(col.size() == 1);

    edm4hep::ReconstructedParticleCollection col2 = std::move(col);
    REQUIRE(col2.size() == 1);
    REQUIRE(col.size() == 0);
  }
  SECTION("Link move") {
    edm4hep::VertexCollection vertices;
    edm4hep::ReconstructedParticleCollection particles;
    edm4hep::VertexRecoParticleLinkCollection links;
    const int kType = 42;
    const int kPDG = 11;
    {
      edm4hep::VertexCollection vertices2;
      vertices2.setID(1);
      edm4hep::ReconstructedParticleCollection particles2;
      particles2.setID(2);
      edm4hep::VertexRecoParticleLinkCollection links2;
      links2.setID(3);

      auto v = vertices2.create();
      v.setType(kType);
      REQUIRE(vertices2.size() == 1);
      REQUIRE(vertices2[0].getType() == kType);
      auto p = particles2.create();
      p.setPDG(kPDG);
      REQUIRE(particles2.size() == 1);
      REQUIRE(particles2[0].getPDG() == kPDG);

      auto l = links2.create();
      l.set(edm4hep::Vertex(v));
      l.set(edm4hep::ReconstructedParticle(p));
      REQUIRE(links2.size() == 1);

      vertices = std::move(vertices2);
      particles = std::move(particles2);
      links = std::move(links2);
    }
    REQUIRE(vertices.size() == 1);
    REQUIRE(vertices[0].getType() == kType);
    REQUIRE(particles.size() == 1);
    REQUIRE(particles[0].getPDG() == kPDG);
    REQUIRE(links.size() == 1);
    REQUIRE(links[0].getFrom().getType() == kType);
    REQUIRE(links[0].getTo().getPDG() == kPDG);
  }
}
