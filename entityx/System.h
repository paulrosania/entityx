/*
 * Copyright (C) 2012 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#pragma once


#include <cstdint>
#include <unordered_map>
#include <utility>
#include <cassert>
#include "entityx/config.h"
#include "entityx/Entity.h"
#include "entityx/Event.h"
#include "entityx/help/NonCopyable.h"


namespace entityx {


class SystemManager;


/**
 * Base System class. Generally should not be directly used, instead see System<Derived>.
 */
class BaseSystem {
 public:
  BaseSystem() = default;
  NONCOPYABLE(BaseSystem);
  virtual ~BaseSystem() = default;

  /**
   * Called once all Systems have been added to the SystemManager.
   *
   * Typically used to set up event handlers.
   */
  virtual void configure(EntityManager &entities, EventManager &events) {
    configure(events);
  }

  /**
   * Legacy configure(). Called by default implementation of configure(EntityManager&, EventManager&).
   */
  virtual void configure(EventManager &events) {}

  /**
   * Apply System behavior.
   *
   * Called every game step.
   */
  virtual void update(EntityManager &entities, EventManager &events, TimeDelta dt) = 0;
};


/**
 * Use this class when implementing Systems.
 *
 * struct MovementSystem : public System<MovementSystem> {
 *   void update(EntityManager &entities, EventManager &events, TimeDelta dt) {
 *     // Do stuff to/with entities...
 *   }
 * }
 */
template <typename Derived>
class System : public BaseSystem {
 public:
  virtual ~System() = default;

private:
  friend class SystemManager;
};


class SystemManager {
 public:
  SystemManager(EntityManager &entity_manager,
                EventManager &event_manager) :
                entity_manager_(entity_manager),
                event_manager_(event_manager) {}
  NONCOPYABLE(SystemManager);

  /**
   * Add a System to the SystemManager.
   *
   * Must be called before Systems can be used.
   *
   * eg.
   * std::shared_ptr<MovementSystem> movement = entityx::make_shared<MovementSystem>();
   * system.add(movement);
   */
  template <typename S>
  void add(std::shared_ptr<S> system) {
    systems_.emplace_back(system);
  }

  /**
   * Add a System to the SystemManager.
   *
   * Must be called before Systems can be used.
   *
   * eg.
   * auto movement = system.add<MovementSystem>();
   */
  template <typename S, typename ... Args>
  std::shared_ptr<S> add(Args && ... args) {
    std::shared_ptr<S> s(new S(std::forward<Args>(args) ...));
    add(s);
    return s;
  }

  /**
   * Call System::update() on all registered systems.
   *
   * Systems are updated in the order they are registered.
   */
  void update_all(TimeDelta dt);

  /**
   * Configure the system. Call after adding all Systems.
   *
   * This is typically used to set up event handlers.
   */
  void configure();

 private:
  bool initialized_ = false;
  EntityManager &entity_manager_;
  EventManager &event_manager_;
  std::vector<std::shared_ptr<BaseSystem>> systems_;
};

}  // namespace entityx
